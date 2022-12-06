/**
 * @file InternalCommunication.cpp
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief
 * @version 0.1
 * @date 2022-11-06
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "InternalCommunication.h"

constexpr uint32_t InternalCommunication::kCommTimeout;
constexpr uint16_t InternalCommunication::kReceiveTimeout;
constexpr uint16_t InternalCommunication::kMinPacketSize;
constexpr uint8_t  InternalCommunication::kPacketHeader1;
constexpr uint8_t  InternalCommunication::kPacketHeader2;
constexpr uint16_t InternalCommunication::kHeader1Index;
constexpr uint16_t InternalCommunication::kHeader2Index;
constexpr uint16_t InternalCommunication::kLengthIndex;

InternalCommunication::InternalCommunication(
                        std::shared_ptr<IUartCommunication> uart_comm)
:   uart_comm_(uart_comm)
{
    receive_timer_ = xTimerCreate(  "InCmdReceiveTimer",
                                    pdMS_TO_TICKS(kReceiveTimeout),
                                    pdFALSE,
                                    this,
                                    TimeoutCallbackEntry);
}

InternalCommunication::~InternalCommunication()
{
}

/**
 * @brief 内部ボードにコマンドを送信する
 *
 * @param dest 送信先ボード
 * @param send_msg 送信メッセージ
 * @return bool 送信成否
 */
bool InternalCommunication::SendInCmdPacket(
                        const std::vector<uint8_t> send_cmd,
                        std::unique_ptr<std::vector<uint8_t>> response)
{
    auto send_packet = MakePacket(send_cmd);


    if (!uart_comm_->SendMsg(send_packet))
    {
        return false;
    }

    /* 受信待ち */
    /* レスポンスタイムアウトセット */
    if (!ReceiveResponse(std::move(response)))
    {
        return false;
    }

    return true;
}

/**
 * @brief コマンドを受信するまでポーリング
 * 
 * @return bool 
 */
bool InternalCommunication::ReceiveResponse(
        const std::unique_ptr<std::vector<uint8_t>> response)
{
    std::vector<uint8_t> recv_buffer;

    IsReceiveTimeout = false;
    xTimerStart(receive_timer_, 0);

    while (1)
    {
        while (!uart_comm_->IsUartEmpty())
        {
            recv_buffer.push_back(uart_comm_->ReadByte());
        }

        if(recv_buffer.size() != 0)
        {
            ParsingResult result;
            
            /* 解析 */
            result = ParsePacket(recv_buffer);

            /* 解析結果処理 */
            switch (result)
            {
            case ParsingResult::kParsing:
                break;
            
            case ParsingResult::kMismatch:
                recv_buffer.erase(recv_buffer.begin());
                break;
            
            case ParsingResult::kComplete:
                xTimerStop(receive_timer_, 0);

                uint16_t packet_length = recv_buffer[kLengthIndex];
                std::copy(  recv_buffer.begin() + 3,
                            recv_buffer.begin() + packet_length - 1,
                            response->begin());

                return true;

            }
        }

        /* タイムアウト */
        if (IsReceiveTimeout)
        {
            return false;
        }
        
        if (uart_comm_->IsUartEmpty())
        {
            uart_comm_->WaitReceiveData(kCommTimeout);
        }
    }
}

std::vector<uint8_t> InternalCommunication::MakePacket(
                                            const std::vector<uint8_t> cmd_msg)
{
	uint16_t packet_size = cmd_msg.size() + kMinPacketSize - 1;
	std::vector<uint8_t> packet;

	packet.reserve(packet_size);
    packet.push_back(kPacketHeader1);
    packet.push_back(kPacketHeader2);
    
    packet.push_back(static_cast<uint8_t>(packet_size & 0xFF));

    packet.insert(packet.end(), cmd_msg.begin(), cmd_msg.end());

    uint8_t checksum = CalcChecksum(packet);
    packet.push_back(checksum);

    return packet;
}


InternalCommunication::ParsingResult InternalCommunication::ParsePacket(
                                        const std::vector<uint8_t> parse_packet)
{
    std::vector<uint8_t> packet = parse_packet;

    /* Header1 チェック */

    if (parse_packet.size() <= kHeader1Index)
    {
        return ParsingResult::kParsing;
    }

    if (parse_packet[kHeader1Index] != kPacketHeader1)
    {
        return ParsingResult::kMismatch;
    }


    /* Header2 チェック */

    if (parse_packet.size() <= kHeader2Index)
    {
        return ParsingResult::kParsing;
    }

    if (parse_packet[kHeader2Index] != kPacketHeader2)
    {
        return ParsingResult::kMismatch;
    }


    /* パケット長 チェック */

    if (parse_packet.size() <= kLengthIndex)
    {
        return ParsingResult::kParsing;
    }

    auto packet_length = static_cast<uint16_t>(parse_packet[kLengthIndex]);

    if (packet_length < kMinPacketSize)
    {
        return ParsingResult::kMismatch;
    }

    if (packet_length <= parse_packet.size())
    {
        return ParsingResult::kParsing;
    }

    
    std::vector<uint8_t> calc_packet;
    std::copy(parse_packet.begin(), parse_packet.end() - 1, 
                calc_packet.begin());

    if (parse_packet[packet_length - 1] != CalcChecksum(calc_packet))
    {
        return ParsingResult::kMismatch;
    }
    
    return ParsingResult::kComplete;
}


void InternalCommunication::TimeoutCallbackEntry(TimerHandle_t xTimer)
{
    auto instance = reinterpret_cast<InternalCommunication*>(pvTimerGetTimerID(xTimer));
    instance->HandleTimeout();
}

void InternalCommunication::HandleTimeout()
{
    IsReceiveTimeout = true;
}


/**
 * @brief チェックサムを計算する
 * 
 * @param a 被加算数
 * @param b 加算数
 * @return uint8_t 計算結果
 */
uint8_t InternalCommunication::AddCheckSum(const uint8_t a, const uint8_t b)
{
    uint16_t sum;
    sum = static_cast<uint16_t>(a) + static_cast<uint16_t>(b);

    return static_cast<uint8_t>(sum & 0x00FF);
}

uint8_t InternalCommunication::CalcChecksum(
                                    const std::vector<uint8_t> calc_packet)
{
    uint8_t sum = 0x00;

    for (auto i = calc_packet.begin(); i != calc_packet.end(); i++)
    {
        sum = AddCheckSum(sum, *i);
    }

    return sum;
    
}
