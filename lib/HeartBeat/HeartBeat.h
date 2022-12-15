/**
 * @file HeartBeat.h
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief 通信状態の監視を担うクラス
 * @version 0.1
 * @date 2022-10-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef HEART_BEAT_H_
#define HEART_BEAT_H_

#include <memory>
#include "FreeRTOS.h"
#include "timers.h"

#include "INotificationUartIrq.h"
#include "IResetIc.h"
#include "IHeartBeat.h"

class HeartBeat: public IHeartBeat
{
public:
    HeartBeat(IResetIc&);
    ~HeartBeat();

    virtual void Init(INotificationUartIrq&);
    virtual bool StartMonitoring();
    virtual bool StopMonitoring();
    virtual bool SetMonitoringTimeout(uint32_t);
    virtual bool IsMonitoringComm();
    virtual uint16_t GetTimeout();
private:
    IResetIc& reset_ic_;
    bool is_monitoring_comm_;
    uint32_t shutdown_timeout_;
    TimerHandle_t monitor_comm_timer_;

    static void TimeoutCallbackEntry(TimerHandle_t);
    void TimeoutCallback();

    static void UartIrqCallbackEntry(std::shared_ptr<void>);
    void TimerReset();
};

#endif /* HEART_BEAT_H_ */