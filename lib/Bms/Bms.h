#ifndef BMS_H_
#define BMS_H_

#include <stdint.h>
#include <memory>
#include <vector>
#include <FreeRTOS.h>
#include <timers.h>
#include <map>
#include <queue>
#include "usart.h"

#include "IUartCommunication.h"
#include "IBms.h"

class Bms: public IBms, public IUartCommunication
{
public:
    Bms(std::shared_ptr<IUartCommunication>, UART_HandleTypeDef*);
    ~Bms();

    virtual void Init();

    /* パラメータクリア関数 */
    virtual bool ClearCycleCount();
    virtual bool ClearAlarm();

    /* パラメータ取得関数 */
    virtual uint8_t  GetRemainingCapacity();
    virtual uint16_t GetCycleCount();
    virtual uint16_t GetCellGroup1Voltage();
    virtual uint16_t GetCellGroup2Voltage();
    virtual uint16_t GetCellGroup3Voltage();
    virtual uint16_t GetCellGroup4Voltage();
    virtual uint16_t GetTotalVoltage();
    virtual uint16_t GetChargingCurrent();
    virtual uint16_t GetDischargeCurrent();
    virtual int16_t  GetTemperature();
    virtual uint16_t GetTotalChargingTime();
    virtual uint16_t GetTotalDischargeTime();
    virtual uint16_t GetTotalChargingTimeAlarm();
    virtual uint16_t GetTotalDischargeTimeAlarm();
    virtual uint16_t GetTemperatureAlarm();
    virtual uint16_t GetSpcAlarm();



private:
    enum class CommCmd: uint8_t;
    struct CmdParsingParam;

    /* 定数宣言 */
    static constexpr uint16_t kCommTimeout = 150;   /* 通信タイムアウト[ms] */ 
    static constexpr uint8_t  kCommHeader = 0xAA;   /* 通信ヘッダ           */

    static constexpr uint32_t kClearNotification    = 0xFFFFFFFF;
    static constexpr uint32_t kNotClearNotification = 0x00000000;
    static constexpr uint32_t kNotificationRecvData = 0x00000001;
    static constexpr uint32_t kNotificationTimeout  = 0x00000002;

    /* 変数宣言 */
    std::map<CommCmd, uint16_t> cmd_length_;
    std::shared_ptr<IUartCommunication> uart_comm_;
    uint8_t recv_data_;
    UART_HandleTypeDef* uart_handle_;
    

    /* 関数宣言 */
    bool SendAsyncCmd(CommCmd);
    std::vector<uint8_t> SendSyncCmd(CommCmd);
    bool ParsingPacket(const CommCmd, std::vector<uint8_t>);
    uint8_t AddChecksum(const uint8_t, const uint8_t);



};

#endif /* BMS_H_ */