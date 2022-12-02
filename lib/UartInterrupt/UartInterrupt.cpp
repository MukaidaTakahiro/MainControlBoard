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


/**
 * @brief Uart割込み時のコールバックの登録
 * 
 * @param huart 割込み元のUARTハンドル
 * @param instance コールバック先のインスタンス
 * @param func コールバック関数
 * @return bool 成否
 */
bool UartInterrupt::RegistCallback(
        UART_HandleTypeDef* huart, 
        CallbackInstance instance,
        CallbackFunc func)
{
    if (instance == nullptr || func == nullptr)
    {
        return false;
    }

    callback_info_list_[huart].instance = instance;
    callback_info_list_[huart].func = func;
    
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
    auto result = callback_info_list_.find(huart);

    /* 登録チェック */
    if (result == callback_info_list_.end())
    {
        return false;
    }
    
    CallbackInstance instance = callback_info_list_[huart].instance;
    CallbackFunc func = callback_info_list_[huart].func;
    
    func(instance);

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

//void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
//{
//    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
//    HAL_Delay(100);
//    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
//    HAL_Delay(100);
//}