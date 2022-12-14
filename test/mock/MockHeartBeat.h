#ifndef HEART_BEAT_H_
#define HEART_BEAT_H_

#include <gmock/gmock.h>
#include <stdint.h>

#include "IHeartBeat.h"

class MockHeartBeat: public IHeartBeat
{
public:
    ~MockHeartBeat() = default;
    MOCK_METHOD1(Init, void(INotificationUartIrq&));
    MOCK_METHOD0(StartMonitoring, bool(void));
    MOCK_METHOD0(StopMonitoring, bool(void));
    MOCK_METHOD1(SetMonitoringTimeout, bool(uint16_t));
    MOCK_METHOD0(IsMonitoringComm, bool(void));
    MOCK_METHOD0(GetTimeout, uint16_t(void));

};

#endif /* HEART_BEAT_H_ */