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
    task_queue_ = xQueueCreate(256, sizeof(uint8_t));
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
    uart_interrupt_->RegistNotificationTask(uart_handle_, &(this->handle_));
    CreateTask();
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
    recv_buffer_.swap(empty);
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
    if (!IsUartEmpty())
    {
        return true;
    }

    if(read_buffer_task_ != nullptr)
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
    //debug
    uint8_t debug_msg[256] = {0};

    std::copy(msg.begin(), msg.end(), debug_msg);


    result = HAL_UART_Transmit(uart_handle_, msg.data(), msg.size(), 
                                HAL_MAX_DELAY);
    return (result == HAL_OK);
}

/* private関数 ****************************************************************/

void InUartCommunication::PerformTask()
{
    while (1)
    {
        uint32_t recv_data;
        xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &recv_data, portMAX_DELAY);
        xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
        recv_buffer_.push(static_cast<uint8_t>(recv_data & 0x000000FF));
        xSemaphoreGive(recv_buffer_mutex_);
        StoreUartData();
    }
}

void InUartCommunication::StoreUartData()
{
    /* 受信通知 */
    if (read_buffer_task_ != nullptr)
    {
        xTaskNotifyGive(read_buffer_task_);
    }
    return;
}
