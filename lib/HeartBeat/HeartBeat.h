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

class HeartBeat: public std::enable_shared_from_this<HeartBeat>
{
public:
    HeartBeat(std::shared_ptr<INotificationUartIrq>, std::shared_ptr<IResetIc>);
    ~HeartBeat();

    virtual bool StartMonitoring();
    virtual bool StopMonitoring();
    virtual bool SetMonitoringTimeout(uint32_t);
private:
    std::shared_ptr<INotificationUartIrq> monitor_comm_;
    std::shared_ptr<IResetIc> reset_ic_;
    bool monitoring_status_;
    uint32_t shutdown_timeout_;
    TimerHandle_t monitor_comm_timer_;

    static void TimeoutCallbackEntry(TimerHandle_t);
    void TimeoutCallback();

    static void UartIrqCallbackEntry(std::shared_ptr<void>);
    void TimerReset();
};

#endif /* HEART_BEAT_H_ */