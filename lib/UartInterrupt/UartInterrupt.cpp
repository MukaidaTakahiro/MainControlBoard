#include "UartInterrupt.h"

std::unique_ptr<UartInterrupt> UartInterrupt::uart_interrupt_ = nullptr;

UartInterrupt::UartInterrupt()
:   regist_num_(0)
{
}

UartInterrupt::~UartInterrupt()
{
}

UartInterrupt& UartInterrupt::GetInstance()
{
    if (uart_interrupt_ == nullptr)
    {
        uart_interrupt_ = std::unique_ptr<UartInterrupt>(new UartInterrupt());
    }
    return *uart_interrupt_;
}

bool UartInterrupt::RegistHandle( UART_HandleTypeDef* huart)
{
    if (regist_num_ >= kUartMaxNum)
    {
        return false;
    }

    recv_info_list_[regist_num_].huart = huart;
    recv_info_list_[regist_num_].buffer_handle = xStreamBufferCreate(64, 1);
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

uint8_t UartInterrupt::GetRecvData(UART_HandleTypeDef* huart)
{
    StreamBufferHandle_t* buffer_handle = nullptr;

    for (uint16_t i = 0; i < regist_num_; i++)
    {
        if (huart == recv_info_list_[i].huart)
        {
            buffer_handle = &(recv_info_list_[i].buffer_handle);
            break;
        }
    }
    if (buffer_handle == nullptr)
    {
        return 0xFF;
    }
    
    uint8_t recv_data;

    xStreamBufferReceive(*buffer_handle, &recv_data, 1, portMAX_DELAY);
    
    return recv_data;
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

    StreamBufferHandle_t* buffer_handle = &(recv_info->buffer_handle);
    uint8_t* recv_data_ptr = &(recv_info->recv_data);
    uint8_t  recv_data = *recv_data_ptr;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* データ取得 */
    xStreamBufferSendFromISR(   *buffer_handle, &recv_data, 1, 
                                &xHigherPriorityTaskWoken);


    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    /* 割込み待ち設定 */
    HAL_UART_Receive_IT(recv_info->huart, recv_data_ptr, 1);

    return true;
}

/******************************************************************************/
/* Uart割込みの実装(クラス定義ではない) ***************************************/
/******************************************************************************/

static UartInterrupt& uart_interrupt = UartInterrupt::GetInstance();

/**
 * @brief 
 * 
 * @param huart 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    //HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
    uart_interrupt.ExcuteRxCpltCallback(huart);
    //HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
	//HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}
