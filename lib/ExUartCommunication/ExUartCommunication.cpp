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
:   TaskBase("ExUartComm", 4, 256),
    uart_interrupt_(uart_interrupt), 
    uart_handle_(uart_handle),
    recv_buffer_task_(nullptr)
    
{
    recv_buffer_mutex_ = xSemaphoreCreateMutex();
    task_buffer_ = xStreamBufferCreate(256, 1);
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
    uart_interrupt_->RegistHandle(uart_handle_);

    CreateTask();
}

/**
 * @brief UARTデータが来るまで待機する
 * 
 * @param timeout 待機時間
 * @return bool true:データ取得, false:タイムアウト
 */
bool ExUartCommunication::WaitReceiveData(const TickType_t timeout)
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
    uint32_t result = ulTaskNotifyTake(pdTRUE, timeout);
    recv_buffer_task_ = nullptr;

    return (result > 0);
    
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

/**
 * @brief バッファをクリアする
 * 
 */
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
                                HAL_MAX_DELAY);
    return (result == HAL_OK);
}

/**
 * @brief データ受信を通知するコールバック
 * 
 * @param instance 
 * @param func 
 * @return bool 
 */
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
    while (1)
    {
        uint8_t recv_data;
        
        recv_data = uart_interrupt_->GetRecvData(uart_handle_);
        xSemaphoreTake(recv_buffer_mutex_, portMAX_DELAY);
        recv_buffer_.push(recv_data);
        xSemaphoreGive(recv_buffer_mutex_);
        StoreUartData();
    }
}
