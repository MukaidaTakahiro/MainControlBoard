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

#include <map>
#include <functional>
#include <vector>
#include <memory>

#include "usart.h"
#include "stm32f4xx_hal.h"


class UartInterrupt
{
public:
    using CallbackInstance = std::shared_ptr<void>;
    using CallbackFunc = std::function<void(std::shared_ptr<void>)>;
    ~UartInterrupt();
    
    static std::shared_ptr<UartInterrupt> GetInstance();
    
    bool RegistCallback(UART_HandleTypeDef*, CallbackInstance, CallbackFunc);
    bool ExcuteRxCpltCallback(UART_HandleTypeDef*);
    
private:
    /* コールバック情報 */
    struct CallbackInfo
    {
        CallbackInstance instance; /* 通知先のインスタンスポインタ */
        CallbackFunc func;         /* コールバック関数             */
    };

    /* Uartハンドル毎の割込みのコールバックリスト */
    using CallbackInfoList = std::map<UART_HandleTypeDef*, CallbackInfo>;

    UartInterrupt();

    static std::shared_ptr<UartInterrupt> uart_interrupt_;
    CallbackInfoList callback_info_list_;

};

#endif /* UART_INTERRUPT_H_ */
