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

HeartBeat::HeartBeat(IResetIc& reset_ic)
:   reset_ic_(reset_ic),
    is_monitoring_comm_(false),
    shutdown_timeout_(UINT16_MAX)
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

void HeartBeat::Init(INotificationUartIrq& notification_uart_irq)
{
    notification_uart_irq.RegistNotifyHeartBeatCallback(this, 
                                                        UartIrqCallbackEntry);
}

/**
 * @brief 監視開始
 * 
 * @return bool 実行成否
 */
bool HeartBeat::StartMonitoring()
{

    /* ステータスチェック */
    if(is_monitoring_comm_ == true)
    {
        return false;
    }
    
    /* タイマースタート */
    is_monitoring_comm_ = true;
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
    if(is_monitoring_comm_ == false)
    {
        return false;
    }

    xTimerStop(monitor_comm_timer_, 0);
    is_monitoring_comm_ = false;
    
    return true;
}

/**
 * @brief タイムアウト時間変更
 * 
 * @param shutdown_timeout 
 * @return bool 
 */
bool HeartBeat::SetMonitoringTimeout(const uint16_t shutdown_timeout)
{
    xTimerStop(monitor_comm_timer_, 0);

    /* 0の場合変更しない */
    if (shutdown_timeout == 0)
    {
        return false;
    }

    /* タイマー時間変更 */
    xTimerChangePeriod(monitor_comm_timer_,
                        pdMS_TO_TICKS(shutdown_timeout), 0);

    shutdown_timeout_ = shutdown_timeout;

    return true;
}

bool HeartBeat::IsMonitoringComm()
{
    return is_monitoring_comm_;
}

uint16_t HeartBeat::GetTimeout()
{
    return shutdown_timeout_;
}

/* Private関数 ****************************************************************/

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
    reset_ic_.ShutdownSystem();
}

/**
 * @brief 外部Uart割込みが発生した時に呼ばれるコールバック関数
 * 
 * @param instance 
 */
void HeartBeat::UartIrqCallbackEntry(void* instance)
{
    reinterpret_cast<HeartBeat*>(instance)->TimerReset();
}

/**
 * @brief タイマーをリセットする関数
 * 
 */
void HeartBeat::TimerReset()
{
    xTimerResetFromISR(monitor_comm_timer_, 0);
}