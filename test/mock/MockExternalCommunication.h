#ifndef MOCK_EXTERNAL_COMMUNICATION_H_
#define MOCK_EXTERNAL_COMMUNICATION_H_

#include <gmock/gmock.h>
#include <vector>
#include "IExternalCommunication.h"

class MockExternalCommunication : public IExternalCommunication
{
public:
    MockExternalCommunication()
    {}
    ~MockExternalCommunication()
    {}

    MOCK_METHOD4(RegistNotionCallback, 
                    bool(   std::shared_ptr<void>, 
                            NotifyRecvCmdCallbackEntry, 
                            NotifyPacketSyntaxErrCallbackEntry, 
                            NotifyChecksumErrCallbackEntry));
    MOCK_METHOD0(Init, bool(void));

    MOCK_METHOD1(SendCmdPacket, bool(std::vector<uint8_t>));

    bool TestRegistNotionCallback(
        std::shared_ptr<void> callback_instance, 
        NotifyRecvCmdCallbackEntry notify_recv_cmd,
        NotifyPacketSyntaxErrCallbackEntry notify_packet_syntax_err,
        NotifyChecksumErrCallbackEntry notify_checksum_err)
    {
        callback_instance_        = callback_instance;
        notify_recv_cmd_          = notify_recv_cmd;
        notify_packet_syntax_err_ = notify_packet_syntax_err;
        notify_checksum_err_      = notify_checksum_err;

        return true;
    }

    std::shared_ptr<void> callback_instance_;
                        /* コマンド取得時のコールバック関数ポインタ           */
    NotifyRecvCmdCallbackEntry notify_recv_cmd_;
                        /* パケット構文エラー通知のコールバック関数ポインタ   */
    NotifyPacketSyntaxErrCallbackEntry notify_packet_syntax_err_;
                        /* チェックサムエラー通知のコールバック関数ポインタ   */
    NotifyChecksumErrCallbackEntry notify_checksum_err_;
};

#endif /* MOCK_EXTERNAL_COMMUNICATION_H_ */