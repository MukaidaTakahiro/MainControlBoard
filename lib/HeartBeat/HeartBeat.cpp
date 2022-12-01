/**
 * @file HeartBeat.cpp
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief 
 * @version 0.1
 * @date 2022-10-25
 * 
 * @copyright Copyrigh:t (c) 2022
 * 
 */

#include "HeartBeat.h"

HeartBeat::HeartBeat(   std::shared_ptr<INotificationUartIrq> monitor_comm, 
                        std::shared_ptr<IResetIc> reset_ic)
:   monitor_comm_(monitor_comm),
    reset_ic_(reset_ic),
    monitoring_status_(false),
    shutdown_timeout_(UINT32_MAX)
{
    /* タイマ―作成 */
    monitor_comm_timer_ = xTimerCreate(
                            "MonitoringComm", 
                            shutdown_timeout_, 
                            pdFALSE, 
                            this, 
                            TimeoutCallbackEntry);
}

HeartBeat::~HeartBeat()
{
}

/**
 * @brief 監視開始
 * 
 * @return bool 実行成否
 */
bool HeartBeat::StartMonitoring()
{

    /* ステータスチェック */
    if(monitoring_status_ == true)
    {
        return false;
    }

    /* UART割込み処理登録 */
    if (monitor_comm_ == nullptr)
    {
        return false;
    }
    
    monitor_comm_->RegistNotifyHeartBeatCallback(shared_from_this(),
                                                UartIrqCallbackEntry);

    /* タイマースタート */
    monitoring_status_ = true;
    xTimerStart(monitor_comm_timer_, 0);
    
    return true;
}

/**
 * @brief タイマー停止
 * 
 * @return bool 成否
 */
bool HeartBeat::StopMonitoring()
{
    if(monitoring_status_ == false)
    {
        return false;
    }

    xTimerStop(monitor_comm_timer_, 0);
    monitoring_status_ = false;
    
    return true;
}

/**
 * @brief タイムアウト時間変更
 * 
 * @param shutdown_timeout 
 * @return bool 
 */
bool HeartBeat::SetMonitoringTimeout(uint32_t shutdown_timeout)
{
    xTimerStop(monitor_comm_timer_, 0);

    /* タイマー時間変更 */
    xTimerChangePeriod(monitor_comm_timer_,
                        shutdown_timeout / portTICK_PERIOD_MS, 0);

    return true;
}

/**
 * @brief タイムアウト処理エントリー関数
 * 
 * @param xTimer 
 */
void HeartBeat::TimeoutCallbackEntry(TimerHandle_t xTimer)
{
    reinterpret_cast<HeartBeat*>(pvTimerGetTimerID(xTimer))->TimeoutCallback();
}

void HeartBeat::TimeoutCallback()
{
    if (reset_ic_ == nullptr)
    {
        return;
    }
    reset_ic_->ShutdownSystem();
}

/**
 * @brief 外部Uart割込みが発生した時に呼ばれるコールバック関数
 * 
 * @param instance 
 */
void HeartBeat::UartIrqCallbackEntry(std::shared_ptr<void> instance)
{
    std::static_pointer_cast<HeartBeat>(instance)->TimerReset();
}

/**
 * @brief タイマーをリセットする関数
 * 
 */
void HeartBeat::TimerReset()
{
    xTimerResetFromISR(monitor_comm_timer_, 0);
}