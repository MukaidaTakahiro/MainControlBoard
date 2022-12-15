#ifndef MOCK_NOTIFICATION_UART_IRQ_H_
#define MOCK_NOTIFICATION_UART_IRQ_H_

#include <gmock/gmock.h>
#include <memory>

#include "INotificationUartIrq.h"

class MockNotificationUartIrq: public INotificationUartIrq
{
public:
    MOCK_METHOD2(   RegistNotifyHeartBeatCallback, 
                    bool(void* const, NotifyHeartBeatCallback));
};

#endif /* MOCK_NOTIFICATION_UART_IRQ_H_ */