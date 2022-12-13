#ifndef I_BMS_H_
#define I_BMS_H_

#include <stdint.h>


class IBms
{
public:

    
    /* パラメータクリア関数 */
    virtual bool ClearCycleCount() = 0;
    virtual bool ClearAlarm()      = 0;

    /* パラメータ取得関数 */
    virtual uint8_t  GetRemainingCapacity()         = 0;
    virtual uint16_t GetCycleCount()                = 0;
    virtual uint16_t GetCellGroup1Voltage()         = 0;
    virtual uint16_t GetCellGroup2Voltage()         = 0;
    virtual uint16_t GetCellGroup3Voltage()         = 0;
    virtual uint16_t GetCellGroup4Voltage()         = 0;
    virtual uint16_t GetTotalVoltage()              = 0;
    virtual uint16_t GetChargingCurrent()           = 0;
    virtual uint16_t GetDischargeCurrent()          = 0;
    virtual int16_t  GetTemperature()               = 0;
    virtual uint16_t GetTotalChargingTime()         = 0;
    virtual uint16_t GetTotalDischargeTime()        = 0;
    virtual uint16_t GetOverChargingVoltageAlarm()  = 0;
    virtual uint16_t GetOverDischargeVoltageAlarm() = 0;
    virtual uint16_t GetTemperatureAlarm()          = 0;
    virtual uint16_t GetSpcAlarm()                  = 0;
};

#endif /* I_BMS_H_ */