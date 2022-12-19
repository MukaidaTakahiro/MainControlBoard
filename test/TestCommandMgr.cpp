#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <memory>

#include "CommandMgr.h"
#include "MockExternalCommunication.h"
#include "MockThrusterMgr.h"
#include "MockExternalBoard.h"
#include "MockBmsMgr.h"
#include "MockHeartBeat.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Args;
using ::testing::ElementsAreArray;
using ::testing::Pointee;
using ::testing::PrintToString;
using ::testing::Invoke;
using ::testing::SaveArg;
using ::testing::SaveArgPointee;
using ::testing::DoAll;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;



class CommandMgrTestFixture: public::testing::Test
{
public:
    using NotifyRecvCmdCallbackEntry 
            = IExternalCommunication::NotifyRecvCmdCallbackEntry;
    using NotifyPacketSyntaxErrCallbackEntry 
            = IExternalCommunication::NotifyPacketSyntaxErrCallbackEntry;
    using NotifyChecksumErrCallbackEntry 
            = IExternalCommunication::NotifyChecksumErrCallbackEntry;
protected:

    std::unique_ptr<CommandMgr>                cmd_mgr_;
    std::unique_ptr<MockExternalCommunication> ex_comm_;
    std::unique_ptr<MockThrusterMgr>           thruster_mgr_;
    std::unique_ptr<MockExternalBoard>         ex_board_;
    std::unique_ptr<MockBmsMgr>                bms_mgr_;
    std::unique_ptr<MockHeartBeat>             heart_beat_;

    virtual void SetUp()
    {

        ex_comm_      = std::make_unique<MockExternalCommunication>();
        thruster_mgr_ = std::make_unique<MockThrusterMgr>();
        ex_board_     = std::make_unique<MockExternalBoard>();
        bms_mgr_      = std::make_unique<MockBmsMgr>();
        heart_beat_   = std::make_unique<MockHeartBeat>();
        cmd_mgr_      = std::make_unique<CommandMgr>(   *ex_comm_,
                                                        *thruster_mgr_,
                                                        *ex_board_,
                                                        *bms_mgr_,
                                                        *heart_beat_);

    }

    virtual void TearDown()
    {
        ex_comm_.reset();
    }
};

class CommandListTestFixture: public::testing::Test
{
public:
    using NotifyRecvCmdCallbackEntry 
            = IExternalCommunication::NotifyRecvCmdCallbackEntry;
    using NotifyPacketSyntaxErrCallbackEntry 
            = IExternalCommunication::NotifyPacketSyntaxErrCallbackEntry;
    using NotifyChecksumErrCallbackEntry 
            = IExternalCommunication::NotifyChecksumErrCallbackEntry;
protected:
    static constexpr uint16_t kThrusterNum = 8;

    /* CmdSetCommMonitor */
    static constexpr uint8_t  kCmdSetCommMonitorVal  = 0x12;
    static constexpr uint16_t kCmdSetCommMonitorSize = 8;
    static constexpr uint16_t kResSetCommMonitorSize = 8;

    /* CmdControl */
    static constexpr uint8_t  kCmdControlVal  = 0x18;
    static constexpr uint16_t kCmdControlSize = 16;
    static constexpr uint16_t kResControlSize = 20;

    std::unique_ptr<CommandMgr>                cmd_mgr_;
    std::unique_ptr<MockExternalCommunication> ex_comm_;
    std::unique_ptr<MockThrusterMgr>           thruster_mgr_;
    std::unique_ptr<MockExternalBoard>         ex_board_;
    std::unique_ptr<MockBmsMgr>                bms_mgr_;
    std::unique_ptr<MockHeartBeat>             heart_beat_;

    void* instance_;
    NotifyRecvCmdCallbackEntry notify_recv_cmd_;

    virtual void SetUp()
    {

        ex_comm_      = std::make_unique<MockExternalCommunication>();
        thruster_mgr_ = std::make_unique<MockThrusterMgr>();
        ex_board_     = std::make_unique<MockExternalBoard>();
        bms_mgr_      = std::make_unique<MockBmsMgr>();
        heart_beat_   = std::make_unique<MockHeartBeat>();
        cmd_mgr_      = std::make_unique<CommandMgr>(   *ex_comm_,
                                                        *thruster_mgr_,
                                                        *ex_board_,
                                                        *bms_mgr_,
                                                        *heart_beat_);

        EXPECT_CALL(*ex_comm_, RegistNotifyCallback(_,_,_,_))
            .Times(1)
            .WillOnce(DoAll(SaveArg<0>(&instance_), 
                            SaveArg<1>(&notify_recv_cmd_), 
                            Return(true)));

        cmd_mgr_->Init();
    }

    virtual void TearDown()
    {
    }
};

/**
 * @brief 初期化処理
 * 
 */
TEST_F(CommandMgrTestFixture, Init_1)
{

    EXPECT_CALL(*ex_comm_, RegistNotifyCallback(_,_,_,_))
        .Times(1);

    cmd_mgr_->Init();

}

/**
 * @brief 未定義コマンド取得
 *        未定義コマンドエラーを発報すること
 * 
 */
TEST_F(CommandMgrTestFixture, ParseCmd_1)
{
    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;
    void* instance;
    NotifyRecvCmdCallbackEntry notify_recv_cmd;

    /* コマンド作成 */
    cmd_msg.push_back(0xFF);

    /* 取得コマンド作成(未定義コマンドエラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kUndefinedCommand));

    EXPECT_CALL(*ex_comm_, RegistNotifyCallback(_,_,_,_))
        .Times(1)
        .WillOnce(DoAll(SaveArg<0>(&instance), 
                        SaveArg<1>(&notify_recv_cmd), 
                        Return(true)));
    EXPECT_CALL(*ex_comm_, SendCmdPacket(expect_msg))
        .Times(1);

    cmd_mgr_->Init();

    notify_recv_cmd(reinterpret_cast<CommandMgr*>(instance), cmd_msg);
}

/**
 * @brief コマンド引数不足
 *        CmdSyntaxErrを発報すること
 * 
 */
TEST_F(CommandMgrTestFixture, ParseCmd_2)
{
    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;
    void* instance;
    NotifyRecvCmdCallbackEntry notify_recv_cmd;

    /* コマンド作成 */
    cmd_msg.push_back(CommandMgr::kCmdControl);
    cmd_msg.push_back(0xFF);

    /* 取得コマンド作成(コマンド構文エラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kCommandSyntaxError));

    EXPECT_CALL(*ex_comm_, RegistNotifyCallback(_,_,_,_))
        .Times(1)
        .WillOnce(DoAll(SaveArg<0>(&instance), 
                        SaveArg<1>(&notify_recv_cmd), 
                        Return(true)));
    EXPECT_CALL(*ex_comm_, SendCmdPacket(expect_msg))
        .Times(1);

    cmd_mgr_->Init();
    notify_recv_cmd(instance, cmd_msg);
}

/**
 * @brief コマンド引数過剰
 *        CmdSyntaxErrを発報すること
 * 
 */
TEST_F(CommandMgrTestFixture, ParseCmd_3)
{

    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;
    void* instance;
    NotifyRecvCmdCallbackEntry notify_recv_cmd;

    /* コマンド作成 */
    uint16_t cmd_heartbeat_size = 16;
    cmd_msg.push_back(CommandMgr::kCmdControl);
    for (uint16_t i = 0; i < cmd_heartbeat_size + 1; i++)
    {
        cmd_msg.push_back(0xFF);
    }

    /* 取得コマンド作成(コマンド構文エラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kCommandSyntaxError));

    EXPECT_CALL(*ex_comm_, RegistNotifyCallback(_,_,_,_))
        .Times(1)
        .WillOnce(DoAll(SaveArg<0>(&instance), 
                        SaveArg<1>(&notify_recv_cmd), 
                        Return(true)));
    EXPECT_CALL(*ex_comm_, SendCmdPacket(expect_msg))
        .Times(1);

    cmd_mgr_->Init();
    notify_recv_cmd(instance, cmd_msg);
}

/**
 * @brief パケット構文エラー通知取得
 *        PacketSyntaxErrを発報すること
 * 
 */
TEST_F(CommandMgrTestFixture, ParseCmd_4)
{

    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;
    void* instance;
    NotifyPacketSyntaxErrCallbackEntry notify_packet_syntax_err;

    /* 取得コマンド作成(コマンド構文エラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kPacketSyntaxError));

    EXPECT_CALL(*ex_comm_, RegistNotifyCallback(_,_,_,_))
        .Times(1)
        .WillOnce(DoAll(SaveArg<0>(&instance), 
                        SaveArg<2>(&notify_packet_syntax_err), 
                        Return(true)));
    EXPECT_CALL(*ex_comm_, SendCmdPacket(expect_msg))
        .Times(1);

    cmd_mgr_->Init();
    notify_packet_syntax_err(instance);
}

/**
 * @brief チェックサムエラー
 * 
 */
TEST_F(CommandMgrTestFixture, ParseCmd_5)
{

    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;
    void* instance;
    NotifyChecksumErrCallbackEntry notify_checksum_err;

    /* 取得コマンド作成(コマンド構文エラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kCheckSumError));

    EXPECT_CALL(*ex_comm_, RegistNotifyCallback(_,_,_,_))
        .Times(1)
        .WillOnce(DoAll(SaveArg<0>(&instance), 
                        SaveArg<3>(&notify_checksum_err), 
                        Return(true)));
    EXPECT_CALL(*ex_comm_, SendCmdPacket(expect_msg))
        .Times(1);

    cmd_mgr_->Init();
    notify_checksum_err(instance);
}


/**
 * @brief 監視ONコマンド取得
 *        タイムアウト時間を設定すること
 *        タイマーを開始すること
 *        応答コマンドを送信すること
 */
TEST_F(CommandListTestFixture, CmdSetCommMonitor_1)
{
    uint8_t monitor_mode = 0x01;
    uint16_t monitor_timeout = 1000;

    std::vector<uint8_t> set_comm_monitor_cmd;
    set_comm_monitor_cmd.push_back(kCmdSetCommMonitorVal);
    set_comm_monitor_cmd.push_back(monitor_mode);
    set_comm_monitor_cmd.push_back(monitor_timeout & 0xFF);
    set_comm_monitor_cmd.push_back((monitor_timeout >> 8) & 0xFF);

    std::vector<uint8_t> set_comm_monitor_res {set_comm_monitor_cmd};
    set_comm_monitor_res[0] = kCmdSetCommMonitorVal + 1;

    EXPECT_CALL(*heart_beat_, SetMonitoringTimeout(monitor_timeout))
        .Times(1);
    EXPECT_CALL(*heart_beat_, StartMonitoring())
        .Times(1);
    EXPECT_CALL(*heart_beat_, IsMonitoringComm())
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(*heart_beat_, GetTimeout())
        .Times(1)
        .WillOnce(Return(monitor_timeout));

    EXPECT_CALL(*ex_comm_, SendCmdPacket(set_comm_monitor_res))
        .Times(1)
        .WillOnce(Return(true));

    notify_recv_cmd_(instance_, set_comm_monitor_cmd);

}


/**
 * @brief 制御コマンド取得
 *        スラスタへ制御値を渡すこと
 *        BOBへ制御パラメータ要求コマンドを送信すること
 *        応答コマンドを送信すること
 */
TEST_F(CommandListTestFixture, CmdControl_1)
{

    std::vector<uint8_t> control_cmd;           
    std::vector<uint8_t> bob_response_cmd;
    std::vector<uint16_t> expect_thruster_power;
    std::vector<uint8_t> expect_control_res;
    std::vector<uint8_t> expect_bob_request_cmd;

    /* 期待されるスラスタ値ベクタ */
    for (uint16_t i = 0; i < kThrusterNum; i++)
    {
        expect_thruster_power.push_back(i * 1000);
    }

    /* コマンド作成 */
    control_cmd.push_back(CommandMgr::kCmdControl);
    for (uint16_t i = 0; i < kThrusterNum; i++)
    {
        /* スラスタ値 */
        /* 下位バイト */
        control_cmd.push_back(expect_thruster_power[i] & 0xFF);
        /* 上位バイト */   
        control_cmd.push_back((expect_thruster_power[i] >> 8) & 0xFF);        
    }

    /* BOBへの要求コマンド */
    expect_bob_request_cmd.push_back(kCmdControlVal);

    /* BOBからの応答メッセージ */
    bob_response_cmd.push_back(kCmdControlVal + 1);
    for (uint16_t i = 0; i < kResControlSize; i++)
    {
        bob_response_cmd.push_back(i & 0xFF);
    }

    /* 期待されるメッセージ */
    expect_control_res.insert(expect_control_res.begin(), 
                bob_response_cmd.begin(), 
                bob_response_cmd.end());

    /* テスト評価登録 *********************************************************/

    /* スラスタ */
    EXPECT_CALL(*thruster_mgr_, OperateThruster(expect_thruster_power))
        .Times(1)
        .WillOnce(Return(true));

    /* 外部ボードへのコマンド送信処理 */
    EXPECT_CALL(*ex_board_, SendCmd(BoardId::kBob, expect_bob_request_cmd, _))
        .Times(1)
        .WillOnce(DoAll(SetArgReferee<2>(expect_control_res), Return(true)));

    /* システム外部への送信処理 */
    EXPECT_CALL(*ex_comm_, SendCmdPacket(expect_control_res))
        .Times(1)
        .WillOnce(Return(true));

    /* テスト対象の関数実行 ***************************************************/
    notify_recv_cmd_(instance_, control_cmd);

}

