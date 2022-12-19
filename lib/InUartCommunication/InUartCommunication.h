/**
 * @file InUartCommunication.h
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief システム内部の通信を担うクラス
 * @version 0.1
 * @date 2022-10-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef IN_UART_COMMUNICATION_H_
#define IN_UART_COMMUNICATION_H_

#include <stdint.h>
#include <vector>
#include <queue>
#include <memory>

#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include "usart.h"

#include "IUartCommunication.h"
#include "UartInterrupt.h"
#include "TaskBase.h"

class InUartCommunication
:   public TaskBase,
    public IUartCommunication
{
public:
    InUartCommunication(UartInterrupt&, 
                        UART_HandleTypeDef* const);
    ~InUartCommunication();

    virtual void Init();

    /* 受信関連 */
    virtual uint8_t ReadByte();
    virtual bool IsUartEmpty();
    virtual void ClearBuffer();
    virtual bool WaitReceiveData(const TickType_t);

    /* 送信関連 */
    virtual bool SendMsg(std::vector<uint8_t>);

private:
    /* 定数宣言 */
    static constexpr uint16_t kRecvDataSize = 256;

    /* メンバ変数 */
    UartInterrupt& uart_interrupt_; /* Uart割込みインスタンス */
    UART_HandleTypeDef* uart_handle_;         /* UART通信ハンドル       */
    uint8_t recv_data_;                     /* UART割込みのデータ格納先       */
    std::queue<uint8_t> recv_buffer_;       /* 受信バッファ                   */
    SemaphoreHandle_t recv_buffer_mutex_;   /* 受信バッファ処理のmutex        */
    TaskHandle_t recv_task_;                /* 受信タスクハンドル             */
    TaskHandle_t read_buffer_task_;         /* データ読み込みタスクハンドル   */
    StreamBufferHandle_t task_buffer_;


    /* メンバ関数 */
    static void HandleUartCallback(UartInterrupt::CallbackInstance);
    void NotifyUartInterrupt();
    virtual void PerformTask();
    void StoreUartData();
};

#endif /* IN_UART_COMMUNICATION_H_ */