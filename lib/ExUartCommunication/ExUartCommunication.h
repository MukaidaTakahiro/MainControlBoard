/**
 * @file ExUartCommunication.h
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief システム外部とのUart通信を担うクラス
 *        シングルトンパターン
 * @version 0.1
 * @date 2022-10-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef EX_UART_COMMUNICATION_H_
#define EX_UART_COMMUNICATION_H_

#include <stdint.h>
#include <vector>
#include <queue>
#include <functional>
#include <memory>

#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include "usart.h"

#include "TaskBase.h"
#include "IUartCommunication.h"
#include "INotificationUartIrq.h"
#include "UartInterrupt.h"

class ExUartCommunication
:   public IUartCommunication, 
    public INotificationUartIrq,
    public std::enable_shared_from_this<ExUartCommunication>,
    public TaskBase
{
public:
    
    ExUartCommunication(std::shared_ptr<UartInterrupt>, UART_HandleTypeDef*);
    ~ExUartCommunication();

    void Init();

    /* 受信関連 */
    virtual bool WaitReceiveData(const TickType_t);
    virtual uint8_t ReadByte();
    virtual bool IsUartEmpty();
    virtual void ClearBuffer();

    /* 送信関連 */
    virtual bool SendMsg(std::vector<uint8_t>);

    /* 通信監視 */
    virtual bool RegistNotifyHeartBeatCallback(std::shared_ptr<void>, 
                                                NotifyHeartBeatCallback);

private:
    /* 定数宣言 */
    static constexpr uint32_t kUartTimeOut = 1000;
    static constexpr uint16_t kRecvDataSize = 256;


    /* メンバ変数 */
    std::shared_ptr<UartInterrupt> uart_interrupt_;
    UART_HandleTypeDef* const uart_handle_;     /* UARTハンドル       */
    
    uint8_t recv_data_;                         /* 受信データ         */
    std::queue<uint8_t> recv_buffer_;           /* 受信データ用キュー */
    SemaphoreHandle_t recv_buffer_mutex_;       /* 受信データ用mutex  */

    volatile TaskHandle_t recv_buffer_task_;
    StreamBufferHandle_t task_buffer_;

    std::shared_ptr<void> uart_callback_instance_; /* UART呼び出し時のコールバックインスタンス */
    NotifyHeartBeatCallback uart_callback_func_;     /* UART呼び出し時のコールバック関数 */
    
    uint16_t read_ptr_;   /* DMA受信バッファの先頭インデックス */

    
    /* メンバ関数 */
    void PerformTask();
    void StoreUartData();
    static void CallbackTaskFunc(std::shared_ptr<void>);
    static void HandleUartCallback(UartInterrupt::CallbackInstance);
    void NotifyUartInterruptCallback();

    
    uint8_t ReadUartData();

};


#endif /* EX_UART_COMMUNICATION_H_ */