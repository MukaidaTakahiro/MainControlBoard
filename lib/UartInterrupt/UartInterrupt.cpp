#include "UartInterrupt.h"

std::shared_ptr<UartInterrupt> UartInterrupt::uart_interrupt_ = nullptr;

UartInterrupt::UartInterrupt()
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

bool UartInterrupt::RegistReceiveQueue( UART_HandleTypeDef* huart, 
                                        QueueHandle_t* queue)
{
    recv_info_list_[huart->Instance].recv_queue = queue;
    return true;
}

bool UartInterrupt::Init()
{
    RecvInfoList::iterator itr;

    for (itr = recv_info_list_.begin(); itr != recv_info_list_.end(); itr++)
    {
        RecvInfo* recv_info = &(itr->second);
        HAL_UART_Receive_IT(itr->first, &(recv_info->recv_data), 1);
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

    
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
    uint8_t* recv_data_ptr = &(recv_info_list_[huart->Instance].recv_data);
    QueueHandle_t* task_queue = recv_info_list_[huart->Instance].recv_queue;
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* データ取得 */
    xQueueSendToBackFromISR(*task_queue, recv_data_ptr, 
                            &xHigherPriorityTaskWoken);

    /* 割込み待ち設定 */
    HAL_UART_Receive_IT(huart, recv_data_ptr, 1);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

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
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
    uart_interrupt->ExcuteRxCpltCallback(huart);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    ;
}
