﻿#ifndef I_NOTIFICATION_UART_IRQ_H_
#define I_NOTIFICATION_UART_IRQ_H_

#include <memory>
#include <functional>


class INotificationUartIrq
{
public:
    using NotifyHeartBeatCallback
            = std::function<void(std::shared_ptr<void>)>;

    virtual bool RegistNotifyHeartBeatCallback( std::shared_ptr<void>, 
                                                NotifyHeartBeatCallback) = 0;
};

#endif /* I_NOTIFICATION_UART_IRQ_H_ */