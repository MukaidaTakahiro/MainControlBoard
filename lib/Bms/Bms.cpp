#include "Bms.h"

/**
 * @brief 通信コマンド
 *
 */
enum class Bms::CommCmd : uint8_t
{
    kGetRemainCapacity = 0x01,          /* 残存容量取得                 */
    kGetCycleCnt = 0x02,                /* サイクルカウント取得         */
    kClearCycleCnt = 0x03,              /* サイクルカウントクリア       */
    kGetCellGroup1Voltage = 0x04,       /* No.1 セル群電圧取得          */
    kGetCellGroup2Voltage = 0x05,       /* No.2 セル群電圧取得          */
    kGetCellGroup3Voltage = 0x06,       /* No.3 セル群電圧取得          */
    kGetCellGroup4Voltage = 0x07,       /* No.4 セル群電圧取得          */
    kGetTotalVoltage = 0x08,            /* トータル電池パック電圧取得   */
    kGetChargingCurrent = 0x09,         /* 充電電流取得                 */
    kGetDischargeCurrent = 0x0A,        /* 放電電流取得                 */
    kGetTemperature = 0x0B,             /* 電池温度取得                 */
    kGetTotalChargingTime = 0x0C,       /* 総充電時間取得               */
    kGetTotalDischargeTime = 0x0D,      /* 総放電時間取得               */
    kGetTotalChargingTimeAlarm = 0x10,  /* セル群過充電電圧アラーム取得 */
    kGetTotalDischargeTimeAlarm = 0x11, /* セル群過放電電圧アラーム取得 */
    kGetTemperatureAlarm = 0x12,        /* 電池温度アラーム取得         */
    kGetSpcAlarm = 0x13,                /* SPC 作動アラーム取得         */
    kClearAlarm = 0x1F                  /* アラームクリア               */
};

struct Bms::CmdParsingParam
{
    std::vector<uint8_t> cmd_msg;
    uint16_t cmd_len;
    uint16_t checksum;

};


Bms::Bms(std::shared_ptr<IUartCommunication> uart_comm, 
            UART_HandleTypeDef* uart_handle)
:   uart_comm_(uart_comm), uart_handle_(uart_handle)
{
    cmd_length_ = 
    {
        {CommCmd::kGetRemainCapacity,          4},
        {CommCmd::kGetCycleCnt,                5},
        {CommCmd::kClearCycleCnt,              4},
        {CommCmd::kGetCellGroup1Voltage,       5},
        {CommCmd::kGetCellGroup2Voltage,       5},
        {CommCmd::kGetCellGroup3Voltage,       5},
        {CommCmd::kGetCellGroup4Voltage,       5},
        {CommCmd::kGetTotalVoltage,            5},
        {CommCmd::kGetChargingCurrent,         5},
        {CommCmd::kGetDischargeCurrent,        5},
        {CommCmd::kGetTemperature,             5},
        {CommCmd::kGetTotalChargingTime,       5},
        {CommCmd::kGetTotalDischargeTime,      5},
        {CommCmd::kGetTotalChargingTimeAlarm,  5},
        {CommCmd::kGetTotalDischargeTimeAlarm, 5},
        {CommCmd::kGetTemperatureAlarm,        5},
        {CommCmd::kGetSpcAlarm,                5},
        {CommCmd::kClearAlarm,                 4}
    };
}

Bms::~Bms()
{
}

/**
 * @brief サイクルクリアする
 *        BMSからの応答は待たない
 * 
 * @return bool 成否
 */
bool Bms::ClearCycleCount()
{
    return SendAsyncCmd(CommCmd::kClearCycleCnt);
}

/**
 * @brief アラームクリアする
 *        BMSからの応答は待たない
 * 
 * @return bool 成否
 */
bool Bms::ClearAlarm()
{
    return SendAsyncCmd(CommCmd::kClearAlarm);
}

uint8_t Bms::GetRemainingCapacity()
{
    uint8_t result = 0x00;

    // 

    SendAsyncCmd(CommCmd::kGetRemainCapacity);

    // タイムアウトセット

    // コマンド待ち


    return result;
}




bool Bms::SendAsyncCmd(CommCmd cmd)
{
    std::vector<uint8_t> send_msg;

    send_msg.push_back(kCommHeader);
    send_msg.push_back(static_cast<uint8_t>(cmd));

    return uart_comm_->SendMsg(send_msg);

}

std::vector<uint8_t> Bms::SendSyncCmd(CommCmd cmd)
{
    std::vector<uint8_t> send_msg;
    uint32_t notification;
    std::vector<uint8_t> response_msg;

    send_msg.push_back(kCommHeader);
    send_msg.push_back(static_cast<uint8_t>(cmd));

    // 割込みセット
    // タイマーセット
    // 受信待ち
    while (1)
    {
        xTaskNotifyWait(kNotClearNotification, kClearNotification, 
                        &notification, portMAX_DELAY);

        if (notification & kNotificationRecvData)
        {
            // コマンド処理
            response_msg.push_back(recv_data_);

            if (response_msg.size() >= cmd_length_[cmd])
            {
                if (ParsingPacket(cmd, response_msg))
                {
                    // タイマーストップ
                    break;
                }
                else
                {
                    response_msg.erase(response_msg.begin());
                }

                HAL_UART_Receive_IT(uart_handle_, &recv_data_, 1);
                
            }
        }

        if (notification & kNotificationTimeout)
        {
            // 割込みクリア
            break;
        }
        
    }

    return response_msg;
    
}


bool Bms::ParsingPacket(const CommCmd cmd, std::vector<uint8_t> packet)
{
    uint8_t checksum = 0x00;

    if (packet.front() != kCommHeader)
    {
        return false;
    }

    checksum = AddChecksum(checksum, packet.front());

    if (packet.front() != static_cast<uint8_t>(cmd))
    {
        return false;
    }

    checksum = AddChecksum(checksum, packet.front());

    for (uint16_t i = 0; i < cmd_length_[cmd] - 3; i++)
    {
        checksum = AddChecksum(checksum, packet.front());
    }

    if (packet.front() != checksum)
    {
        return false;
    }

    return true;
    
}

uint8_t Bms::AddChecksum(const uint8_t a, const uint8_t b)
{
    uint16_t sum;
    sum = (uint16_t)a + (uint16_t)b;

    return (uint8_t)(sum & 0x00FF);
}