#ifndef I_UART_COMMUNICATION_H_
#define I_UART_COMMUNICATION_H_

#include <stdint.h>
#include <vector>
#include <functional>
#include <memory>

class IUartCommunication
{
public:

    virtual ~IUartCommunication()
    {}

    virtual void Init() = 0;

    /* 受信関連 */
    virtual bool WaitReceiveData(TickType_t) = 0;
    virtual uint8_t ReadByte() = 0;
    virtual bool IsUartEmpty() = 0;
    virtual void ClearBuffer() = 0;
    

    /* 送信関連 */
    virtual bool SendMsg(std::vector<uint8_t>) = 0;

};

#endif /* I_UART_COMMUNICATION_H_ */