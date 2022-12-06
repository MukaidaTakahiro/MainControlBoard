#include "UartInterrupt.h"

std::shared_ptr<UartInterrupt> UartInterrupt::uart_interrupt_ = nullptr;

UartInterrupt::UartInterrupt()
:   regist_num_(0)
{
}

UartInterrupt::~UartInterrupt()
{
}

std::shared_ptr<UartInterrupt> UartInterrupt::GetInstance()
{
    if (!uart_interrupt_)
    {
        uart_interrupt_ = std::shared_ptr<UartInterrupt>(new UartInterrupt());
    }
    return uart_interrupt_;
}

bool UartInterrupt::RegistNotificationTask( UART_HandleTypeDef* huart, 
                                            TaskHandle_t* notification_task)
{
    if (regist_num_ >= kUartMaxNum)
    {
        return false;
    }

    recv_info_list_[regist_num_].huart = huart;
    recv_info_list_[regist_num_].notification_task = notification_task;
    regist_num_++;
    return true;
}

bool UartInterrupt::Init()
{
    for (uint16_t i = 0; i < regist_num_; i++)
    {
        RecvInfo* recv_info = &recv_info_list_[i];
        HAL_UART_Receive_IT(recv_info->huart, &(recv_info->recv_data), 1);
    }
    return true;
}


/**
 * @brief Uart割込み実行処理
 * 
 * @param huart 割込みハンドラ
 * @return bool 実行成否
 */
inline bool UartInterrupt::ExcuteRxCpltCallback(UART_HandleTypeDef* huart)
{
    RecvInfo* recv_info = nullptr;

    for (uint16_t i = 0; i < regist_num_; i++)
    {
        if (huart == recv_info_list_[i].huart)
        {
            recv_info = &recv_info_list_[i];
            break;
        }
    }

    if (recv_info == nullptr)
    {
        return false;
    }

    TaskHandle_t* notification_task = recv_info->notification_task;
    uint8_t* recv_data_ptr = &(recv_info->recv_data);
    uint8_t  recv_data = *recv_data_ptr;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* データ取得 */
    xTaskNotifyFromISR( *notification_task, recv_data,
                        eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken);


    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    /* 割込み待ち設定 */
    HAL_UART_Receive_IT(recv_info->huart, recv_data_ptr, 1);

    return true;
}

/******************************************************************************/
/* Uart割込みの実装(クラス定義ではない) ***************************************/
/******************************************************************************/

static std::shared_ptr<UartInterrupt> uart_interrupt = UartInterrupt::GetInstance();

/**
 * @brief 
 * 
 * @param huart 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_interrupt->ExcuteRxCpltCallback(huart);
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}
