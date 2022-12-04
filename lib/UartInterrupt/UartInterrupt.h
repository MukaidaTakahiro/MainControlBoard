/**
 * @file UartInterrupt.h
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief  Uart割込みを担う
 * @version 0.1
 * @date 2022-10-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef UART_INTERRUPT_H_
#define UART_INTERRUPT_H_

#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>

#include "FreeRTOS.h"
#include "queue.h"
#include "usart.h"
#include "stm32f4xx_hal.h"

class UartInterrupt
{
public:
    using CallbackInstance = std::shared_ptr<void>;
    using CallbackFunc = std::function<void(CallbackInstance)>;
    ~UartInterrupt();
    
    static std::shared_ptr<UartInterrupt> GetInstance();
    
    bool RegistReceiveQueue(UART_HandleTypeDef*, QueueHandle_t*);
    bool Init();
    bool ExcuteRxCpltCallback(UART_HandleTypeDef*);
    
private:

    struct RecvInfo
    {
        uint8_t recv_data;
        QueueHandle_t* recv_queue;
    };

    /* Uartハンドル毎の割込みのコールバックリスト */
    using RecvInfoList = std::unordered_map<UART_HandleTypeDef*, 
                                                RecvInfo>;


    UartInterrupt();

    static std::shared_ptr<UartInterrupt> uart_interrupt_;
    RecvInfoList recv_info_list_;

    //debug
    CallbackInstance instance_;
    CallbackFunc     func_;

};


#endif /* UART_INTERRUPT_H_ */
