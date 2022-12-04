#include "ExUartCommunication.h"
//debug
#include "main.h"
constexpr uint32_t ExUartCommunication::kUartTimeOut;
constexpr uint16_t ExUartCommunication::kRecvDataSize;

/**
 * @brief Construct a new Ex Uart Communication::ExUartCommunication object
 *
 */
ExUartCommunication::ExUartCommunication(
                    std::shared_ptr<UartInterrupt> uart_interrupt,
                    UART_HandleTypeDef* uart_handle)
:   TaskBase("ExUartComm", 4, 512),
    uart_interrupt_(uart_interrupt), 
    uart_handle_(uart_handle),
    recv_task_(nullptr),
    recv_buffer_task_(nullptr)
    
{
    recv_buffer_mutex_ = xSemaphoreCreateMutex();
    task_que_ = xQueueCreate(256, sizeof(uint8_t));
}


ExUartCommunication::~ExUartCommunication()
{
}

/**
 * @brief 初期化処理
 *        
 */
void ExUartCommunication::Init()
{
    /* Uart割込みコールバック関数登録 */
    uart_interrupt_->RegistReceiveQueue(uart_handle_,  
                                        &task_que_);

    CreateTask();
}

/**
 * @brief UARTデータが来るまで待機する
 * 
 * @param timeout 待機時間
 * @return bool true:データ取得, false:タイムアウト
 */
bool ExUartCommunication::WaitReceiveData(TickType_t timeout)
{
    if (!IsUartEmpty())
    {
        return true;
    }
    
    if (recv_buffer_task_ != nullptr)
    {
        return false;
    }

    recv_buffer_task_ = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTake(pdTRUE, timeout);
    recv_buffer_task_ = nullptr;

    return true;
    
}

/**
 * @brief Uartデータを1バイト取得する
 * 
 * @return uint8_t 取得データ
 */
uint8_t ExUartCommunication::ReadByte()
{
    uint8_t read_char = 0x00;
    if (!IsUartEmpty())
    {
        xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
        read_char = recv_buffer_.front();
        recv_buffer_.pop();
        xSemaphoreGive(recv_buffer_mutex_);
    }
    return read_char;
}

void ExUartCommunication::ClearBuffer()
{
    xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
    std::queue<uint8_t>().swap(recv_buffer_);
    xSemaphoreGive(recv_buffer_mutex_);
}

/**
 * @brief 外部にメッセージを送信する
 *
 * @param msg       送信データ
 * @return true     送信成功
 * @return false    送信失敗
 */
bool ExUartCommunication::SendMsg(std::vector<uint8_t> msg)
{
    HAL_StatusTypeDef result;   /* 送信結果 */

    result = HAL_UART_Transmit(uart_handle_, msg.data(), msg.size(), 
                                30000);
    return (result == HAL_OK);
}

bool ExUartCommunication::RegistNotifyHeartBeatCallback(
                                                std::shared_ptr<void> instance,
                                                NotifyHeartBeatCallback func)
{
    if (instance == nullptr || func == nullptr)
    {
        return false;
    }
    
    uart_callback_instance_ = instance;
    uart_callback_func_ = func;

    return true;
}

/**
 * @brief 受信バッファが空状態を判定する
 * 
 * @return bool true:空 false:空ではない
 */
bool ExUartCommunication::IsUartEmpty()
{
    uint16_t buffer_size;

    xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
    buffer_size = recv_buffer_.size();
    xSemaphoreGive(recv_buffer_mutex_);

    return (buffer_size == 0);
}

/**
 * @brief UART割込み処理関数
 * 
 * @param instance コールバック先のインスタンス
 */
void ExUartCommunication::HandleUartCallback(
                                    UartInterrupt::CallbackInstance instance)
{
    std::static_pointer_cast<ExUartCommunication>(instance)
                                                ->NotifyUartInterruptCallback();
}

/**
 * @brief 割込み通知処理
 * 
 */
inline void ExUartCommunication::NotifyUartInterruptCallback()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* データ取得 */
    xQueueSendToBackFromISR(task_que_, &recv_data_, 0);

    /* 割込み待ち設定 */
    HAL_UART_Receive_IT(uart_handle_, &recv_data_, 1);
    
    vTaskNotifyGiveFromISR(recv_task_, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief UARTデータ格納処理
 *        データ待ちのタスクがあれば通知する
 */
inline void ExUartCommunication::StoreUartData()
{
    
    /* 受信通知 */
    if (recv_buffer_task_ != nullptr)
    {
        xTaskNotifyGive(recv_buffer_task_);
    }

    return;
}

/**
 * @brief タスク処理
 *        UART割込みが発生したらデータをバッファに格納する
 */
void ExUartCommunication::PerformTask()
{
    recv_task_ = xTaskGetCurrentTaskHandle();
    
    HAL_UART_Receive_IT(uart_handle_, &recv_data_, 1);
    
    while (1)
    {
        uint8_t recv_data;
        xQueueReceive(task_que_, &recv_data, portMAX_DELAY);
        recv_buffer_.push(recv_data);
        StoreUartData();
    }
}
