/*
 * excomm.cpp
 *
 *  Created on: Sep 26, 2022
 *      Author: 向田隆浩
 */

#include "ExternalCommunication.h"
#ifdef _UNIT_TEST
#include "gtest/gtest.h"
#endif /* _UNIT_TEST */
#ifdef DEBUG
#include "main.h"
#endif

constexpr uint8_t ExternalCommunication::kPacketHeader1;
constexpr uint8_t ExternalCommunication::kPacketHeader2;


/***********************************************************/
/* Publicメソッド定義                                      */
/***********************************************************/

/**
 * @brief Construct a new External Communication:: External Communication object
 * 
 * @param uart_comm UART通信クラスのインスタンス
 * @param excomm_task 外部通信タスククラスのインスタンス
 */
ExternalCommunication::ExternalCommunication(
        IUartCommunication& uart_comm)
:   TaskBase("Excomm", 3, 512),
    uart_comm_(uart_comm), 
    callback_instance_(nullptr),
    notify_recv_cmd_(nullptr), 
    notify_packet_syntax_err_(nullptr), 
    notify_checksum_err_(nullptr)
{
    /* 解析状態初期化 */
    state_ = PacketPasingState::kWaitingHeader1;
    cmd_msg_.reserve(kMaxCmdSize);

    return;
}

ExternalCommunication::~ExternalCommunication()
{
}

/**
 * @brief 初期化処理(タスク登録)
 * 
 * @return bool 
 */
bool ExternalCommunication::Init()
{
    CreateTask();

    return true;
}

/**
 * @brief コールバック関数の登録
 * 
 * @param callback_instance         コールバック先のインスタンス
 * @param notify_recv_cmd           コマンド受信時のコールバック関数
 * @param notify_packet_syntax_err  構文エラー時のコールバック関数
 * @param notify_checksum_err       チェックサムエラー時のコールバック関数
 * @return bool 
 */
bool ExternalCommunication::RegistNotifyCallback(
                    void* callback_instance, 
                    NotifyRecvCmdCallbackEntry notify_recv_cmd,
                    NotifyPacketSyntaxErrCallbackEntry notify_packet_syntax_err,
                    NotifyChecksumErrCallbackEntry notify_checksum_err)
{
    if (callback_instance == nullptr 
        || notify_recv_cmd == nullptr
        || notify_packet_syntax_err == nullptr 
        || notify_checksum_err == nullptr)
    {
        return false;
    }

    callback_instance_ = callback_instance;
    notify_recv_cmd_ = notify_recv_cmd;
    notify_packet_syntax_err_ = notify_packet_syntax_err;
    notify_checksum_err_ = notify_checksum_err;
    
    return true;

}


/**
 * @brief コマンドを通信パケットに変換して送信する
 * 
 * @param cmd コマンドメッセージ
 * @return bool true:成功 false:構文エラー
 */
bool ExternalCommunication::SendCmdPacket(const std::vector<uint8_t> cmd)
{
    /* サイズチェック */
    if (cmd.size() > kMaxCmdSize || cmd.size() <= 0)
    {
        return false;
    }

    uint16_t packet_length = cmd.size() + kMinPacketSize - 1;
    std::vector<uint8_t> packet; /* 送信パケット */
    packet.reserve(packet_length);

    /* header-1 */
    packet.push_back(kPacketHeader1);

    /* header-2 */
    packet.push_back(kPacketHeader2);

    /* パケット長格納 */
    packet.push_back(static_cast<uint8_t>(packet_length));

    /* コマンド格納 */
    packet.insert(packet.end(), cmd.begin(), cmd.end());

    /* チェックサム格納 */
    uint8_t checksum = CalcChecksum(packet);
    packet.push_back(checksum);

    //HAL_GPIO_WritePin(Debug1_GPIO_Port, Debug1_Pin, GPIO_PIN_RESET);

    /* パケット送信 */
    return uart_comm_.SendMsg(packet);
}

/***********************************************************/
/* Privateメソッド定義                                     */
/***********************************************************/

void ExternalCommunication::PerformTask()
{
    std::vector<uint8_t> parse_buffer;

    while (1)
    {
        ParsePacket();
    }    
}

inline void ExternalCommunication::ParsePacket()
{
    static std::vector<uint8_t> parse_buffer;
    parse_buffer.reserve(kMaxCmdSize);

    uart_comm_.WaitReceiveData(portMAX_DELAY);

    while (!uart_comm_.IsUartEmpty())
    {
        parse_buffer.push_back(uart_comm_.ReadByte());
    }

    auto result = ParseSyntax(parse_buffer);

    while (result == PacketParsingResult::kMismatch)
    {
        parse_buffer.erase(parse_buffer.begin());
        result = ParseSyntax(parse_buffer);
    }

    uint16_t packet_length;

    switch (result)
    {
    case PacketParsingResult::kParsing:
    case PacketParsingResult::kMismatch:
        break;
    case PacketParsingResult::kReceived:
        //HAL_GPIO_WritePin(Debug1_GPIO_Port, Debug1_Pin, GPIO_PIN_SET);
        packet_length = static_cast<uint16_t>(parse_buffer[kIndexLength]);
        notify_recv_cmd_(   callback_instance_,
                            {parse_buffer.begin() + 3,
                            parse_buffer.begin() + packet_length - 1});
        parse_buffer.erase( parse_buffer.begin(),
                            parse_buffer.begin() + packet_length);

        //HAL_GPIO_TogglePin(Debug1_GPIO_Port, Debug1_Pin);
        break;

    case PacketParsingResult::kSyntaxErr:
        notify_packet_syntax_err_(callback_instance_);
        parse_buffer.erase( parse_buffer.begin(),
                            parse_buffer.begin() + kIndexLength + 1);
        break;

    case PacketParsingResult::kChecksumErr:
        notify_checksum_err_(callback_instance_);

        packet_length = static_cast<uint16_t>(parse_buffer[kIndexLength]);
        parse_buffer.erase( parse_buffer.begin(),
                            parse_buffer.begin() + packet_length);
        break;
    default:
        break;
    }
}

ExternalCommunication::PacketParsingResult 
ExternalCommunication::ParseSyntax(const std::vector<uint8_t> parse_array)
{

    /* Header1 */
    if (parse_array.size() <= kIndexHeader1)
    {
        return PacketParsingResult::kParsing;
    }

    if (parse_array[kIndexHeader1] != kPacketHeader1)
    {
        return PacketParsingResult::kMismatch;
    }
    
    /* Header2 */
    if (parse_array.size() <= kIndexHeader2)
    {
        return PacketParsingResult::kParsing;
    }

    if (parse_array[kIndexHeader2] != kPacketHeader2)
    {
        return PacketParsingResult::kMismatch;
    }

    /* Length */
    if (parse_array.size() <= kIndexLength)
    {
        return PacketParsingResult::kParsing;
    }

    uint16_t packet_length = static_cast<uint16_t>(parse_array[kIndexLength]);

    if (packet_length < kMinPacketSize)
    {
        return PacketParsingResult::kSyntaxErr;
    }

    /* Cmd */
    if (parse_array.size() < packet_length)
    {
        return PacketParsingResult::kParsing;
    }

    /* Checksum */
    uint8_t checksum = CalcChecksum({parse_array.begin(), 
                                    parse_array.begin() + packet_length - 1});

    if (checksum != parse_array[packet_length - 1])
    {
        return PacketParsingResult::kChecksumErr;
    }
    
    return PacketParsingResult::kReceived;    
}


uint8_t ExternalCommunication::CalcChecksum(const std::vector<uint8_t> array)
{
    uint8_t sum = 0x00;
    for (uint16_t i = 0; i < array.size(); i++)
    {
        sum = AddChecksum(sum, array[i]);
    }
    
    return sum;
}

/**
 * @brief チェックサムを計算する
 * 
 * @param a 被加算数
 * @param b 加算数
 * @return uint8_t 計算結果
 */
uint8_t ExternalCommunication::AddChecksum(const uint8_t a, const uint8_t b)
{
    uint16_t sum;
    sum = static_cast<uint16_t>(a) + static_cast<uint16_t>(b);

    return static_cast<uint8_t>(sum & 0x00FF);
}
