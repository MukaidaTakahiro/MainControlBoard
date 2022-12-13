/**
 * @file ExternalCommunication.h
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief LLVC外部の通信を担うクラス
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef EXTERNALCOMMUNICATION_H_
#define EXTERNALCOMMUNICATION_H_

#include <vector>
#include <stdint.h>
#include <memory>

#include "TaskBase.h"
#include "IUartCommunication.h"
#include "IExternalCommunication.h"


class ExternalCommunication 
:   public IExternalCommunication, public TaskBase
{
public:

    ExternalCommunication(std::shared_ptr<IUartCommunication>);
    virtual ~ExternalCommunication();

    bool Init();

    bool RegistNotifyCallback(  std::shared_ptr<void>, 
                                NotifyRecvCmdCallbackEntry,
                                NotifyPacketSyntaxErrCallbackEntry,
                                NotifyChecksumErrCallbackEntry);

    bool SendCmdPacket(std::vector<uint8_t>);

private:

    /* 定数宣言 */
                                /* 最小パケットサイズ                         */
                                /* header1+header2+パケット長+cmd値+checksum  */
    static constexpr int16_t kMinPacketSize = 5;
                                /* パケット最大サイズ                         */
    static constexpr int16_t kPacketMaxSize = 255;
                                /* 最大コマンドサイズ                         */
    static constexpr int16_t kMaxCmdSize = kPacketMaxSize - kMinPacketSize + 1;

    static constexpr uint8_t kPacketHeader1 = 0xFF; /* header-1 */
    static constexpr uint8_t kPacketHeader2 = 0x00; /* header-2 */

    static constexpr uint16_t kIndexHeader1 = 0;
    static constexpr uint16_t kIndexHeader2 = 1;
    static constexpr uint16_t kIndexLength = 2;
    
    
    enum class PacketParsingResult
    {
        kParsing,
        kReceived,
        kMismatch,
        kSyntaxErr,
        kChecksumErr
    };

    enum class PacketPasingState
    {
        kWaitingHeader1,       /* header1取得待ち       */
        kWaitingHeader2,       /* header2取得待ち       */
        kWaitingPakcketLength, /* パケット長取得待ち    */
        kGettingCmdData,       /* コマンドデータ取得中  */
        kWaitingCheckSum       /* チェックサム          */
    };

    /* メンバ変数宣言 */
                        /* UART通信クラス                                     */
    std::shared_ptr<IUartCommunication> uart_comm_;
                        /* コールバック関数のインスタンス                     */
    std::shared_ptr<void> callback_instance_;
                        /* コマンド取得時のコールバック関数ポインタ           */
    NotifyRecvCmdCallbackEntry notify_recv_cmd_;
                        /* パケット構文エラー通知のコールバック関数ポインタ   */
    NotifyPacketSyntaxErrCallbackEntry notify_packet_syntax_err_;
                        /* チェックサムエラー通知のコールバック関数ポインタ   */
    NotifyChecksumErrCallbackEntry notify_checksum_err_;
    PacketPasingState state_;         /* 解析状態                    */
    uint8_t checksum_;               /* チェックサム値              */
    uint16_t recv_data_cnt_;          /* 受信したデータサイズ        */
    uint16_t packet_length_;          /* パケット長                  */
    std::vector<uint8_t> cmd_msg_;    /* cmdメッセージ               */

    /* メンバ関数宣言 */
    virtual void PerformTask();
    void ParsePacket();
    PacketParsingResult ParseSyntax(const std::vector<uint8_t>);
    uint8_t AddChecksum(const uint8_t, const uint8_t);
    uint8_t CalcChecksum(const std::vector<uint8_t>);

#ifdef _UNIT_TEST
public:
    friend void CallPerformTask(std::shared_ptr<ExternalCommunication>);
#endif /* _UNIT_TEST */
};

#endif /* EXTERNALCOMMUNICATION_H_ */
