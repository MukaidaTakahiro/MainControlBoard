#ifndef I_HEART_BEAT_H_
#define I_HEART_BEAT_H_

#include <stdint.h>
#include "INotificationUartIrq.h"

class IHeartBeat
{
public:
    virtual ~IHeartBeat() = default;
    
    virtual void Init(INotificationUartIrq&) = 0;
    virtual bool StartMonitoring() = 0;
    virtual bool StopMonitoring() = 0;
    virtual bool SetMonitoringTimeout(uint16_t) = 0;
    virtual bool IsMonitoringComm() = 0;
    virtual uint16_t GetTimeout() = 0;
};

#endif /* I_HEART_BEAT_H_ */