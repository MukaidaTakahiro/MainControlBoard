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
#include "stream_buffer.h"
#include "stm32f4xx_hal.h"

class UartInterrupt
{
public:
    using CallbackInstance = std::shared_ptr<void>;
    using CallbackFunc = std::function<void(CallbackInstance)>;
    ~UartInterrupt();
    
    static std::shared_ptr<UartInterrupt> GetInstance();
    
    bool RegistHandle(UART_HandleTypeDef*);
    uint8_t GetRecvData(UART_HandleTypeDef*);
    bool Init();
    bool ExcuteRxCpltCallback(UART_HandleTypeDef*);
    
private:
    static constexpr uint16_t kUartMaxNum = 6;
    struct RecvInfo
    {
        UART_HandleTypeDef* huart;
        uint8_t recv_data;
        StreamBufferHandle_t buffer_handle;
    };

    static std::shared_ptr<UartInterrupt> uart_interrupt_;
    RecvInfo recv_info_list_[kUartMaxNum];
    uint16_t regist_num_;
    
    
    UartInterrupt();
};


#endif /* UART_INTERRUPT_H_ */
