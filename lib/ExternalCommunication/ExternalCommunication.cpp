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

constexpr uint8_t ExternalCommunication::kPacketHeader1;
constexpr uint8_t ExternalCommunication::kPacketHeader2;

static uint8_t AddCheckSum(uint8_t a, uint8_t b);

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
        std::shared_ptr<IUartCommunication> uart_comm)
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
                    std::shared_ptr<void> callback_instance, 
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



/***********************************************************/
/* Privateメソッド定義                                     */
/***********************************************************/

void ExternalCommunication::PerformTask()
{
    while (1)
    {
        while (!uart_comm_->IsUartEmpty())
        {
            PacketParsingResult result;
            auto read_data = uart_comm_->ReadByte();
            
            result = ParsePacket(read_data);

            switch (result)
            {
            case PacketParsingResult::kReceived:
                notify_recv_cmd_(callback_instance_, cmd_msg_);
                break;
            case PacketParsingResult::kSyntaxErr:
                notify_packet_syntax_err_(callback_instance_);
                break;
            case PacketParsingResult::kChecksumErr:
                notify_checksum_err_(callback_instance_);
                break;
            default:
                break;
            }
            
        }
        
        uart_comm_->WaitReceiveData(portMAX_DELAY);
    }    
}

/**
 * @brief 
 * 
 * @param cmd コマンドメッセージ
 * @return bool true:成功 false:構文エラー
 */
bool ExternalCommunication::SendCmdPacket(std::vector<uint8_t> cmd)
{
    std::vector<uint8_t> packet; /* 送信パケット */
    uint8_t checksum = 0x00; /* チェックサム */

    /* サイズチェック */
    if (cmd.size() > kMaxCmdSize || cmd.size() <= 0)
    {
        return false;
    }
    

    /* header-1 */
    packet.push_back(kPacketHeader1);
    checksum = AddCheckSum(checksum, packet.back());

    /* header-2 */
    packet.push_back(kPacketHeader2);
    checksum = AddCheckSum(checksum, packet.back());

    /* パケット長格納 */
    packet.push_back((uint8_t)(cmd.size() + kMinPacketSize - 1));
    checksum = AddCheckSum(checksum, packet.back());

    /* コマンド格納 */
    for (uint16_t i = 0; i < cmd.size(); i++)
    {
        packet.push_back(cmd[i]);
        checksum = AddCheckSum(checksum, packet.back());
    }

    /* チェックサム格納 */
    packet.push_back(checksum);

    /* パケット送信 */
    return uart_comm_->SendMsg(packet);
}


/**
 * @brief 取得したパケットからコマンドを抽出する
 * 
 */
ExternalCommunication::PacketParsingResult 
    ExternalCommunication::ParsePacket(uint8_t read_data)
{

    PacketParsingResult result;

    /* パケットの解析 */
    switch (state_)
    {
    case PacketPasingState::kWaitingHeader1:
        
        /* 初期化 */
        recv_data_cnt_ = 0;
        checksum_ = 0x00;
        cmd_msg_.clear();

        /* Header-1を取得したか？ */
        if (read_data == kPacketHeader1)
        {
            recv_data_cnt_++;
            checksum_ = AddCheckSum(checksum_, read_data);
            state_ = PacketPasingState::kWaitingHeader2;
        }

        result = PacketParsingResult::kPasing;

        break;

    case PacketPasingState::kWaitingHeader2:

        /* Header-2を取得したか？ */
        if (read_data == kPacketHeader2)
        {
            recv_data_cnt_++;
            checksum_ = AddCheckSum(checksum_, read_data);
            state_ = PacketPasingState::kWaitingPakcketLength;
        }
        else
        {
            /* Header-2を取得できなければ最初から */
            state_ = PacketPasingState::kWaitingHeader1;
        }

        result = PacketParsingResult::kPasing;

        break;

    case PacketPasingState::kWaitingPakcketLength:

        packet_length_ = (uint16_t)read_data;

        /* パケット長が最小値より小さければエラー */
        if (packet_length_ < kMinPacketSize)
        {
            state_ = PacketPasingState::kWaitingHeader1;
            result = PacketParsingResult::kSyntaxErr;
            break;
        }

        recv_data_cnt_++;
        checksum_ = AddCheckSum(checksum_, read_data);
        state_ = PacketPasingState::kGettingCmdData;
        result = PacketParsingResult::kPasing;
        break;

    case PacketPasingState::kGettingCmdData:
        cmd_msg_.push_back(read_data);

        recv_data_cnt_++;
        checksum_ = AddCheckSum(checksum_, read_data);
        
        if (cmd_msg_.size() + 4 == packet_length_)
        {
            state_ = PacketPasingState::kWaitingCheckSum;
        }

        result = PacketParsingResult::kPasing;

        break;

    case PacketPasingState::kWaitingCheckSum:

        if (read_data != checksum_)
        {
            result = PacketParsingResult::kChecksumErr;
        }
        else
        {
            result = PacketParsingResult::kReceived;
        }

        state_ = PacketPasingState::kWaitingHeader1;

        break;

    default:
        state_ = PacketPasingState::kWaitingHeader1;
        result = PacketParsingResult::kPasing;
        break;
    }

    return result;
}



/**
 * @brief チェックサムを計算する
 * 
 * @param a 被加算数
 * @param b 加算数
 * @return uint8_t 計算結果
 */
static uint8_t AddCheckSum(uint8_t a, uint8_t b)
{
    uint16_t sum;
    sum = (uint16_t)a + (uint16_t)b;

    return (uint8_t)(sum & 0x00FF);
}
