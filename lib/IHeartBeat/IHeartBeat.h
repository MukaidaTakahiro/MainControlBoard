#ifndef I_HEART_BEAT_H_
#define I_HEART_BEAT_H_

#include <stdint.h>

class IHeartBeat
{
public:
    virtual ~IHeartBeat() = default;
    
    virtual bool StartMonitoring() = 0;
    virtual bool StopMonitoring() = 0;
    virtual bool SetMonitoringTimeout(uint16_t) = 0;
};

#endif /* I_HEART_BEAT_H_ */