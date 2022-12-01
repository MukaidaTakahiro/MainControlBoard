/**
 * @file InUartCommunication.cpp
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "InUartCommunication.h"



/**
 * @brief Construct a new In Uart Communication:: In Uart Communication object
 * 
 * @param uart_handle 通信先とのUartHandle
 */
InUartCommunication::InUartCommunication(
                        const std::shared_ptr<UartInterrupt> uart_interrupt,
                        UART_HandleTypeDef* const uart_handle)
:   TaskBase("InUartComm", 4, 256),
    uart_interrupt_(uart_interrupt), 
    uart_handle_(uart_handle)
{
    /* mutexの作成 */
    recv_buffer_mutex_ = xSemaphoreCreateMutex();
}

InUartCommunication::~InUartCommunication()
{
}

/**
 * @brief 初期化処理
 *        Uart割込みにコールバック関数を登録
 * 
 */
void InUartCommunication::Init()
{
    uart_interrupt_->RegistCallback(uart_handle_, shared_from_this(), 
                                    HandleUartCallback);
}

/**
 * @brief 受信バッファが空か判定する
 * 
 * @return bool true:空, false:空ではない
 */
bool InUartCommunication::IsUartEmpty()
{
    uint16_t buffer_size;

    xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
    buffer_size = recv_buffer_.size();
    xSemaphoreGive(recv_buffer_mutex_);

    return (buffer_size == 0);
}

/**
 * @brief 受信バッファをクリア
 * 
 */
void InUartCommunication::ClearBuffer()
{
    std::queue<uint8_t> empty;
    xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
    recv_buffer_.swap( empty );
    xSemaphoreGive(recv_buffer_mutex_);
}

/**
 * @brief Uartデータを1バイト取得する
 * 
 * @param timeout タイムアウト時間[Tick]
 * @return bool true: 成功, false: タイムアウト
 */
uint8_t InUartCommunication::ReadByte()
{
    uint8_t read_data = 0x00;

    if (!IsUartEmpty())
    {
        xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
        read_data = recv_buffer_.front();
        recv_buffer_.pop();
        xSemaphoreGive(recv_buffer_mutex_);
    }

    return read_data;
}

bool InUartCommunication::WaitReceiveData(const TickType_t timeout)
{
    if(read_buffer_task_ == nullptr)
    {
        return false;
    }

    uint32_t ulNotificationValue;

    read_buffer_task_ = xTaskGetCurrentTaskHandle();
    ulNotificationValue = ulTaskNotifyTake(pdTRUE, timeout);
    read_buffer_task_ = nullptr;

    if (ulNotificationValue != 1)
    {
        return false;
    }

    return true;
}

bool InUartCommunication::SendMsg(std::vector<uint8_t> msg)
{
    HAL_StatusTypeDef result;   /* 送信結果 */

    result = HAL_UART_Transmit(uart_handle_, msg.data(), msg.size(), 
                                kUartTimeOut);
    return (result == HAL_OK);
}

/* private関数 ****************************************************************/

/**
 * @brief Uart割込みコールバック関数
 * 
 * @param instance 
 */
void InUartCommunication::HandleUartCallback(
                                    UartInterrupt::CallbackInstance instance)
{
    std::static_pointer_cast<InUartCommunication>(instance)
                                                        ->NotifyUartInterrupt();
}

/**
 * @brief 受信タスクに割込み発生を通知
 * 
 */
void InUartCommunication::NotifyUartInterrupt()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(recv_task_, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void InUartCommunication::PerformTask()
{
    recv_task_ = xTaskGetCurrentTaskHandle();
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* 受信処理 */
        StoreUartData();
    }
}

void InUartCommunication::StoreUartData()
{
    /* データ取得 */
    xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
    recv_buffer_.push(recv_data_);
    xSemaphoreGive(recv_buffer_mutex_);

    /* 受信通知 */
    if (read_buffer_task_ != nullptr)
    {
        xTaskNotifyGive(read_buffer_task_);
    }

    /* 割込み待ち設定 */
    HAL_UART_Receive_IT(uart_handle_, &recv_data_, 1);
    return;
}
