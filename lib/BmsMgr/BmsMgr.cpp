#include "BmsMgr.h"

BmsMgr::BmsMgr(std::unique_ptr<IBms> bms)
:   TaskBase("BmsMgr", 1, 128), bms_(std::move(bms))
{
    bms_param_mutex_ = xSemaphoreCreateMutex();
}

void BmsMgr::Init()
{
    update_param_timer_ = xTimerCreate(
                            "BmsParamUpdateTimer", 
                            pdMS_TO_TICKS(kBmsParamUpdateCycle), 
                            pdFALSE, 
                            this, 
                            TimeoutEntryPoint);
    
    xTimerStart(update_param_timer_, 0);
}

BmsMgr::BmsParam BmsMgr::GetBmsParam()
{
    BmsParam response;

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    response = bms_param_;
    xSemaphoreGive(bms_param_mutex_);

    return response;
}


/* Private関数 ****************************************************************/

void BmsMgr::PerformTask()
{
    while (1)
    {
        UpdateBmsParam();
    }
}

inline void BmsMgr::UpdateBmsParam()
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.remaining_capacity = bms_->GetRemainingCapacity();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.cycle_cnt = bms_->GetCycleCount();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.cell_group1_voltage = bms_->GetCellGroup1Voltage();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.cell_group2_voltage = bms_->GetCellGroup2Voltage();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.cell_group3_voltage = bms_->GetCellGroup3Voltage();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.cell_group4_voltage = bms_->GetCellGroup4Voltage();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.total_voltage = bms_->GetTotalVoltage();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.charging_current = bms_->GetChargingCurrent();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.discharge_current = bms_->GetDischargeCurrent();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.temperature = bms_->GetTemperature();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.total_charging_time = bms_->GetTotalChargingTime();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.total_discharge_time = bms_->GetTotalDischargeTime();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.overcharging_voltage_alarm 
        = bms_->GetOverChargingVoltageAlarm();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.overdischarge_voltage_alarm 
        = bms_->GetOverDischargeVoltageAlarm();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.temperature_alarm = bms_->GetTemperatureAlarm();
    xSemaphoreGive(bms_param_mutex_);

    xSemaphoreTake(bms_param_mutex_, portMAX_DELAY);
    bms_param_.spc_alarm = bms_->GetSpcAlarm();
    xSemaphoreGive(bms_param_mutex_);

    return;
}

void BmsMgr::TimeoutEntryPoint(TimerHandle_t xTimer)
{
    reinterpret_cast<BmsMgr*>(pvTimerGetTimerID(xTimer))->RequestParamUpdate();
}

void BmsMgr::RequestParamUpdate()
{
    xTaskNotifyGive(this->handle_);
}