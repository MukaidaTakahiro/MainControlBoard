#include "gtest/gtest.h"
#include <gmock/gmock.h>

#include "CommandMgr.h"
#include <MockExternalCommunication.h>
#include <MockThruster.h>
#include <MockBmsMgr.h>

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

/**
 * @brief コンストラクタ成功
 * 
 */

TEST(CommandMgr, Constractor_1)
{
    auto excomm = std::make_shared<MockExternalCommunication>();

    EXPECT_CALL(*excomm, RegistNotionCallback(_,_,_,_))
        .Times(0);

    CommandMgr cmd_mgr(excomm, &device);
}

/**
 * @brief 初期化処理
 * 
 */
TEST(CommandMgr, Init_1)
{
    MockExternalCommunication excomm;
    

    EXPECT_CALL(excomm, RegistNotionCallback(_,_,_,_))
        .Times(1)
        .WillOnce(Invoke(
                    &excomm, 
                    MockExternalCommunication::TestRegistNotionCallback));

    CommandMgr cmd_mgr(&excomm, &device);
    cmd_mgr.Init();
}

/**
 * @brief 未定義コマンド取得
 * 
 */
TEST(CommandMgr, ParseCmd_1)
{
    MockExternalCommunication excomm;
    
    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;

    /* コマンド作成 */
    cmd_msg.push_back(0xFF);

    /* 取得コマンド作成(未定義コマンドエラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kUndefinedCommand));


    EXPECT_CALL(excomm, RegistNotionCallback(_,_,_,_))
        .Times(1)
        .WillOnce(Invoke(
                    &excomm, 
                    MockExternalCommunication::TestRegistNotionCallback));
    EXPECT_CALL(excomm, SendCmdPacket(expect_msg))
        .Times(1);

    CommandMgr cmd_mgr(&excomm, &device);
    cmd_mgr.Init();
    excomm.notion_recv_cmd_(excomm.callback_instance_, cmd_msg);
}

/**
 * @brief コマンド引数不足
 * 
 */
TEST(CommandMgr, ParseCmd_2)
{
    MockExternalCommunication excomm;
    
    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;

    /* コマンド作成 */
    cmd_msg.push_back(CommandMgr::kCmdControl);
    cmd_msg.push_back(0xFF);

    /* 取得コマンド作成(コマンド構文エラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kCommandSyntaxError));


    EXPECT_CALL(excomm, RegistNotionCallback(_,_,_,_))
        .Times(1)
        .WillOnce(Invoke(
                    &excomm, 
                    MockExternalCommunication::TestRegistNotionCallback));
    EXPECT_CALL(excomm, SendCmdPacket(expect_msg))
        .Times(1);

    CommandMgr cmd_mgr(&excomm, &device);
    cmd_mgr.Init();
    excomm.notion_recv_cmd_(excomm.callback_instance_, cmd_msg);
}


/**
 * @brief コマンド引数過剰
 * 
 */
TEST(CommandMgr, ParseCmd_3)
{
    MockExternalCommunication excomm;
    
    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;

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


    EXPECT_CALL(excomm, RegistNotionCallback(_,_,_,_))
        .Times(1)
        .WillOnce(Invoke(
                    &excomm, 
                    MockExternalCommunication::TestRegistNotionCallback));
    EXPECT_CALL(excomm, SendCmdPacket(expect_msg))
        .Times(1);

    CommandMgr cmd_mgr(&excomm, &device);
    cmd_mgr.Init();
    excomm.notion_recv_cmd_(excomm.callback_instance_, cmd_msg);
}

/**
 * @brief パケット構文エラー発生
 * 
 */
TEST(CommandMgr, ParseCmd_4)
{
    MockExternalCommunication excomm;
    
    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;

    /* 取得コマンド作成(コマンド構文エラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kPacketSyntaxError));

    EXPECT_CALL(excomm, RegistNotionCallback(_,_,_,_))
        .Times(1)
        .WillOnce(Invoke(
                    &excomm, 
                    MockExternalCommunication::TestRegistNotionCallback));
    EXPECT_CALL(excomm, SendCmdPacket(expect_msg))
        .Times(1);

    CommandMgr cmd_mgr(&excomm, &device);
    cmd_mgr.Init();
    excomm.notion_packet_syntax_err_(excomm.callback_instance_);
}

/**
 * @brief チェックサムエラー
 * 
 */
TEST(CommandMgr, ParseCmd_5)
{
    MockExternalCommunication excomm;
    
    std::vector<uint8_t> cmd_msg;
    std::vector<uint8_t> expect_msg;

    /* 取得コマンド作成(コマンド構文エラー) */
    expect_msg.push_back(CommandMgr::kCmdError);
    expect_msg.push_back(
        static_cast<uint8_t>(CmdError::ErrorType::kCheckSumError));

    EXPECT_CALL(excomm, RegistNotionCallback(_,_,_,_))
        .Times(1)
        .WillOnce(Invoke(
                    &excomm, 
                    MockExternalCommunication::TestRegistNotionCallback));
    EXPECT_CALL(excomm, SendCmdPacket(expect_msg))
        .Times(1);

    CommandMgr cmd_mgr(&excomm, &device);
    cmd_mgr.Init();
    excomm.notion_checksum_err_(excomm.callback_instance_);
}

/**
 * @brief ハートビートコマンド
 * 
 */
TEST(CommandMgr, CmdControl_1)
{
    MockExternalCommunication excomm;
    
    std::vector<uint8_t> cmd_msg;
    std::vector<int16_t> expect_thruster_power;
    IDevice::HighPriorityParam high_pri_param;
    std::vector<uint8_t> expect_msg;

    /* コマンド作成 */
    uint16_t cmd_heartbeat_size = 16;
    cmd_msg.push_back(CommandMgr::kCmdControl);
    for (uint16_t i = 0; i < cmd_heartbeat_size; i += 2)
    {
        /* スラスタ値の設定 */
        cmd_msg.push_back(i / 2 + 1);   /* 下位バイト */
        cmd_msg.push_back(0x00);        /* 上位バイト */
    }
    
    /* 期待されるスラスタ値ベクタ */
    for (uint16_t i = 0; i < 8; i++)
    {
        expect_thruster_power.push_back(i + 1);
    }
    
    /* ダミーパラメータ */
    high_pri_param.roll = 1000;
    high_pri_param.pitch = 2000;
    high_pri_param.yaw = 3000;
    high_pri_param.depth = 4000;
    high_pri_param.bob_switch_status = 0x11;
    for (uint16_t i = 0; i < 5; i++)
    {
        high_pri_param.bob_switch_voltage[i] = 5000;
        high_pri_param.bob_switch_current[i] = 6000;
    }
    high_pri_param.prb_switch_status = 0x22;
    for (uint16_t i = 0; i < 2; i++)
    {
        high_pri_param.prb_switch_voltage[i] = 7000;
        high_pri_param.prb_switch_current[i] = 8000;
    }
    high_pri_param.eob_switch_status = 0x33;
    for (uint16_t i = 0; i < 5; i++)
    {
        high_pri_param.eob_switch_voltage[i] = 9000;
        high_pri_param.eob_switch_current[i] = 3333;
    }
    
    /* 期待されるメッセージ */
    expect_msg.push_back(CommandMgr::kCmdControl + 1);
    expect_msg.push_back(high_pri_param.roll & 0xFF);
    expect_msg.push_back((high_pri_param.roll >> 8) & 0xFF);
    expect_msg.push_back(high_pri_param.pitch & 0xFF);
    expect_msg.push_back((high_pri_param.pitch >> 8) & 0xFF);
    expect_msg.push_back(high_pri_param.yaw & 0xFF);
    expect_msg.push_back((high_pri_param.yaw >> 8) & 0xFF);
    expect_msg.push_back(high_pri_param.depth & 0xFF);
    expect_msg.push_back((high_pri_param.depth >> 8) & 0xFF);

    expect_msg.push_back(high_pri_param.bob_switch_status);
    for (uint16_t i = 0; i < 5; i++)
    {
        expect_msg.push_back(high_pri_param.bob_switch_voltage[i] & 0xFF);
        expect_msg.push_back((high_pri_param.bob_switch_voltage[i] >> 8) & 0xFF);
    }    
    for (uint16_t i = 0; i < 5; i++)
    {
        expect_msg.push_back(high_pri_param.bob_switch_current[i] & 0xFF);
        expect_msg.push_back((high_pri_param.bob_switch_current[i] >> 8) & 0xFF);
    }

    expect_msg.push_back(high_pri_param.prb_switch_status);
    for (uint16_t i = 0; i < 2; i++)
    {
        expect_msg.push_back(high_pri_param.prb_switch_voltage[i] & 0xFF);
        expect_msg.push_back((high_pri_param.prb_switch_voltage[i] >> 8) & 0xFF);
    }    
    for (uint16_t i = 0; i < 2; i++)
    {
        expect_msg.push_back(high_pri_param.prb_switch_current[i] & 0xFF);
        expect_msg.push_back((high_pri_param.prb_switch_current[i] >> 8) & 0xFF);
    }

    expect_msg.push_back(high_pri_param.eob_switch_status);
    for (uint16_t i = 0; i < 5; i++)
    {
        expect_msg.push_back(high_pri_param.eob_switch_voltage[i] & 0xFF);
        expect_msg.push_back((high_pri_param.eob_switch_voltage[i] >> 8) & 0xFF);
    }    
    for (uint16_t i = 0; i < 5; i++)
    {
        expect_msg.push_back(high_pri_param.eob_switch_current[i] & 0xFF);
        expect_msg.push_back((high_pri_param.eob_switch_current[i] >> 8) & 0xFF);
    }


    EXPECT_CALL(excomm, RegistNotionCallback(_,_,_,_))
        .Times(1)
        .WillOnce(Invoke(
                    &excomm, 
                    MockExternalCommunication::TestRegistNotionCallback));
    EXPECT_CALL(device, OperateThruster(expect_thruster_power))
        .Times(1);
    EXPECT_CALL(device, GetHighPriorityParam())
        .Times(1)
        .WillOnce(Return(high_pri_param));
    
    std::vector<uint8_t> test;
    EXPECT_CALL(excomm, SendCmdPacket(_))
        .WillOnce(DoAll(SaveArg<0>(&test), Return(true)));

    CommandMgr cmd_mgr(&excomm, &device);
    cmd_mgr.Init();
    excomm.notion_recv_cmd_(excomm.callback_instance_, cmd_msg);

    for (uint16_t i = 0; i < test.size(); i++)
    {
        printf("%02d, test:%3d, expect:%2d\n", i, test[i], expect_msg[i]);
    }

    EXPECT_EQ(test, expect_msg);
}