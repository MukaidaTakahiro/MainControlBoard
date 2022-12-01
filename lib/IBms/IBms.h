#ifndef I_BMS_H_
#define I_BMS_H_

#include <stdint.h>


class IBms
{
public:
    struct BmsParam
    {
        uint8_t  remaining_capacity;          /* 残存容量 [%]               */
        uint16_t cycle_cnt;                   /* サイクルカウント [cycle]   */
        uint16_t cell_group1_voltage;         /* No.1 セル群電圧 [mV]       */
        uint16_t cell_group2_voltage;         /* No.2 セル群電圧 [mV]       */
        uint16_t cell_group3_voltage;         /* No.3 セル群電圧 [mV]       */
        uint16_t cell_group4_voltage;         /* No.4 セル群電圧 [mV]       */
        uint16_t total_voltage;               /* トータル電池パック電圧[mV] */
        uint16_t charging_current;            /* 充電電流 [mA]              */
        uint16_t discharge_current;           /* 放電電流 [mA]              */
        int16_t  temperature;                 /* 電池温度 [℃]              */
        uint16_t total_charging_time;         /* 総充電時間 [h]             */
        uint16_t total_discharge_time;        /* 総放電時間 [h]             */
        uint8_t  overcharging_voltage_alarm;  /* セル群過充電電圧アラーム   */
        uint8_t  overdischarge_voltage_alarm; /* セル群過放電電圧アラーム   */
        uint8_t  temperature_alarm;           /* 電池温度アラーム           */
        uint8_t  spc_alarm;                   /* SPC 作動アラーム           */
    };
    
    /* パラメータクリア関数 */
    virtual bool ClearCycleCount() = 0;
    virtual bool ClearAlarm()      = 0;

    /* パラメータ取得関数 */
    virtual uint8_t  GetRemainingCapacity()       = 0;
    virtual uint16_t GetCycleCount()              = 0;
    virtual uint16_t GetCellGroup1Voltage()       = 0;
    virtual uint16_t GetCellGroup2Voltage()       = 0;
    virtual uint16_t GetCellGroup3Voltage()       = 0;
    virtual uint16_t GetCellGroup4Voltage()       = 0;
    virtual uint16_t TotalVoltage()               = 0;
    virtual uint16_t GetChargingCurrent()         = 0;
    virtual uint16_t GetDischargeCurrent()        = 0;
    virtual int16_t  GetTemperature()             = 0;
    virtual uint16_t GetTotalChargingTime()       = 0;
    virtual uint16_t GetTotalDischargeTime()      = 0;
    virtual uint16_t GetTotalChargingTimeAlarm()  = 0;
    virtual uint16_t GetTotalDischargeTimeAlarm() = 0;
    virtual uint16_t GetTemperatureAlarm()        = 0;
    virtual uint16_t GetSpcAlarm()                = 0;
};

#endif /* I_BMS_H_ */