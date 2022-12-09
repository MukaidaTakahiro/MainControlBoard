#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <ExternalCommunication.h>
#include "MockUartCommunication.h"
#include "MockCommandMgr.h"

using ::testing::_;
using ::testing::Args;
using ::testing::ElementsAreArray;
using ::testing::InSequence;
using ::testing::Pointee;
using ::testing::PrintToString;
using ::testing::Return;

/**
 * @brief コンストラクタテスト
 *
 */
TEST(ExternalCommunication, constractor_1)
{

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;
    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(0);

    ExternalCommunication excomm(&u_comm, &ex_task);
}

/**
 * @brief 受信データなし
 *
 */
TEST(ExternalCommunication, ParsePacket_1)
{
    uint16_t count = 5;

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;
    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);
    EXPECT_CALL(u_comm, IsUartEmpty())
        .Times(count)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(u_comm, ReadByte())
        .Times(0);

    for (uint16_t i = 0; i < count; i++)
    {
        ex_task.TestCallbackTask();
    }
}

/**
 * @brief コマンド値のみ
 *
 */
TEST(ExternalCommunication, ParsePacket_2)
{
    std::vector<uint8_t> msg = {0xFF, 0x00, 0x05, 0x55, 0x59};
    uint16_t cnt = 0;
    std::vector<uint8_t> recv_cmd = {0x55};

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);
    excomm.RegistNotionCallback(
        &cmd,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);
    EXPECT_CALL(u_comm, IsUartEmpty())
        .Times(msg.size())
        .WillRepeatedly(Return(false));
    EXPECT_CALL(cmd, ParseCmd(recv_cmd))
        .Times(1);
    EXPECT_CALL(cmd, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(cmd, IssueCheckSumErr())
        .Times(0);

    EXPECT_CALL(u_comm, ReadByte())
        .Times(msg.size())
        .WillOnce(Return(msg[cnt++]))
        .WillOnce(Return(msg[cnt++]))
        .WillOnce(Return(msg[cnt++]))
        .WillOnce(Return(msg[cnt++]))
        .WillOnce(Return(msg[cnt++]));

    for (int i = 0; i < msg.size(); i++)
    {
        ex_task.TestCallbackTask();
    }
}

/**
 * @brief 引数1個のコマンド
 *
 */

TEST(ExternalCommunication, ParsePacket_3)
{
    // InSequence s;
    uint16_t msg_len = 6;
    std::vector<uint8_t> msg;
    uint8_t msg_cnt = 0;
    uint16_t cnt = 0;
    std::vector<uint8_t> recv_cmd;

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    msg.push_back(0xFF);
    msg.push_back(0x00);
    msg.push_back(msg_len);
    for (uint16_t i = msg.size(); i < msg_len - 1; i++)
    {
        msg.push_back(0x00);
        recv_cmd.push_back(0x00);
    }
    msg.push_back(msg[2] - 1);

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);
    excomm.RegistNotionCallback(
        &cmd,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(u_comm, IsUartEmpty())
        .Times(msg.size())
        .WillRepeatedly(Return(false));

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(u_comm, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(cmd, ParseCmd(recv_cmd))
        .Times(1);
    EXPECT_CALL(cmd, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(cmd, IssueCheckSumErr())
        .Times(0);
    for (int i = 0; i < msg.size(); i++)
    {
        ex_task.TestCallbackTask();
    }
}

/**
 * @brief 最長コマンド
 *
 */

TEST(ExternalCommunication, ParsePacket_4)
{
    uint16_t msg_len = 255;
    std::vector<uint8_t> msg;
    std::vector<uint8_t> expect_cmd(msg_len - 4, 0x00);

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;
    /* メッセージ作成 */
    msg.push_back(0xFF);
    msg.push_back(0x00);
    msg.push_back(msg_len);
    msg.insert(msg.end(), expect_cmd.begin(), expect_cmd.end());
    msg.push_back(msg[2] - 1);

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);
    excomm.RegistNotionCallback(
        &cmd,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(u_comm, IsUartEmpty())
        .Times(msg.size())
        .WillRepeatedly(Return(false));
    EXPECT_CALL(cmd, ParseCmd(expect_cmd))
        .Times(1);
    EXPECT_CALL(cmd, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(cmd, IssueCheckSumErr())
        .Times(0);

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(u_comm, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    for (int i = 0; i < msg_len; i++)
    {
        ex_task.TestCallbackTask();
    }
}

/**
 * @brief パケット文法エラー
 *
 */
TEST(ExternalCommunication, ParsePacket_5)
{
    // InSequence s;
    uint16_t msg_len = 6;
    std::vector<uint8_t> msg;
    uint8_t msg_cnt = 0;
    uint16_t cnt = 0;
    std::vector<uint8_t> recv_cmd;

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    msg.push_back(0xFF);
    msg.push_back(0x00);
    msg.push_back(3); /* パケット長を最小値以下にする */
    for (uint16_t i = msg.size(); i < msg_len - 1; i++)
    {
        msg.push_back(0x00);
        recv_cmd.push_back(0x00);
    }
    msg.push_back(msg[2] - 1);

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);
    excomm.RegistNotionCallback(
        &cmd,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(u_comm, IsUartEmpty())
        .Times(msg.size())
        .WillRepeatedly(Return(false));

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(u_comm, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(cmd, ParseCmd(_))
        .Times(0);
    EXPECT_CALL(cmd, IssuePacketSyntaxErr())
        .Times(1);
    EXPECT_CALL(cmd, IssueCheckSumErr())
        .Times(0);
    for (int i = 0; i < msg.size(); i++)
    {
        ex_task.TestCallbackTask();
    }
}

/**
 * @brief チェックサムエラー
 *
 */
TEST(ExternalCommunication, ParsePacket_6)
{
    // InSequence s;
    uint16_t msg_len = 6;
    std::vector<uint8_t> msg;
    uint8_t msg_cnt = 0;
    uint16_t cnt = 0;
    std::vector<uint8_t> recv_cmd;

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    msg.push_back(0xFF);
    msg.push_back(0x00);
    msg.push_back(msg_len);
    for (uint16_t i = msg.size(); i < msg_len - 1; i++)
    {
        msg.push_back(0x00);
        recv_cmd.push_back(0x00);
    }
    msg.push_back(msg[2] - 2); /* チェックサムを間違えさせる */

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);
    excomm.RegistNotionCallback(
        &cmd,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(u_comm, IsUartEmpty())
        .Times(msg.size())
        .WillRepeatedly(Return(false));

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(u_comm, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(cmd, ParseCmd(_))
        .Times(0);
    EXPECT_CALL(cmd, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(cmd, IssueCheckSumErr())
        .Times(1);
    for (int i = 0; i < msg.size(); i++)
    {
        ex_task.TestCallbackTask();
    }
}

/**
 * @brief パケット始端コード以外を取得させる
 *
 */
TEST(ExternalCommunication, ParsePacket_7)
{
    // InSequence s;
    uint16_t msg_len = 6;
    std::vector<uint8_t> msg = {0x55, 0x44, 0x33}; /* 最初にゴミを入れておく */
    std::vector<uint8_t> recv_cmd;

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    msg.push_back(0xFF);
    msg.push_back(0x00);
    msg.push_back(msg_len);
    for (uint16_t i = msg.size(); i < msg_len + 2; i++)
    {
        msg.push_back(0x00);
        recv_cmd.push_back(0x00);
    }
    msg.push_back(msg[5] - 1);

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);
    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);
    excomm.RegistNotionCallback(
        &cmd,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(u_comm, IsUartEmpty())
        .Times(msg.size())
        .WillRepeatedly(Return(false));

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(u_comm, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(cmd, ParseCmd(recv_cmd))
        .Times(1);
    EXPECT_CALL(cmd, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(cmd, IssueCheckSumErr())
        .Times(0);
    for (int i = 0; i < msg.size(); i++)
    {
        ex_task.TestCallbackTask();
    }
}

/**
 * @brief パケット始端コード2以外を取得させる
 *
 */
TEST(ExternalCommunication, ParsePacket_8)
{
    uint16_t msg_len = 6;
    std::vector<uint8_t> invalid_msg = {0xFF, 0x55, 0xFF, 0x44};
    std::vector<uint8_t> msg;
    std::vector<uint8_t> expect_cmd;

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    msg.push_back(0xFF);
    msg.push_back(0x00);
    msg.push_back(msg_len);
    for (uint16_t i = msg.size(); i < msg_len - 1; i++)
    {
        msg.push_back(0x00);
        expect_cmd.push_back(0x00);
    }
    msg.push_back(msg[2] - 1);

    msg.insert(msg.begin(), invalid_msg.begin(), invalid_msg.end());

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);
    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);
    excomm.RegistNotionCallback(
        &cmd,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);
        
    EXPECT_CALL(u_comm, IsUartEmpty())
        .Times(msg.size())
        .WillRepeatedly(Return(false));

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(u_comm, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(cmd, ParseCmd(expect_cmd))
        .Times(1);
    EXPECT_CALL(cmd, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(cmd, IssueCheckSumErr())
        .Times(0);
    for (int i = 0; i < msg.size(); i++)
    {
        ex_task.TestCallbackTask();
    }
}

/******************************************************************************/
/* SendMsgテスト     **********************************************************/
/******************************************************************************/

/**
 * @brief コマンド値のみ送信
 *
 */
TEST(ExternalCommunication, SendMsg_1)
{
    std::vector<uint8_t> msg = {0x44};
    std::vector<uint8_t> expect_msg;
    
    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    expect_msg.push_back(0xFF);
    expect_msg.push_back(0x00);
    expect_msg.push_back(0x05);
    expect_msg.insert(expect_msg.end(), msg.begin(), msg.end());
    expect_msg.push_back(0x48);

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);

    EXPECT_CALL(u_comm, SendMsg(expect_msg))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(excomm.SendCmdPacket(msg));
}

/**
 * @brief 引数1のコマンドを送る
 *
 */
TEST(ExternalCommunication, SendMsg_2)
{
    std::vector<uint8_t> msg = {0x44, 0x00};
    std::vector<uint8_t> expect_msg;
    
    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    expect_msg.push_back(0xFF);
    expect_msg.push_back(0x00);
    expect_msg.push_back(0x06);
    expect_msg.insert(expect_msg.end(), msg.begin(), msg.end());
    expect_msg.push_back(0x49);

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);

    EXPECT_CALL(u_comm, SendMsg(expect_msg))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(excomm.SendCmdPacket(msg));
}

/**
 * @brief 最長コマンドを送る
 *
 */
TEST(ExternalCommunication, SendMsg_3)
{
    std::vector<uint8_t> msg(251, 0x00);
    msg[0] = 0x44;
    std::vector<uint8_t> expect_msg;
    
    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    expect_msg.push_back(0xFF);
    expect_msg.push_back(0x00);
    expect_msg.push_back(0xFF);
    expect_msg.insert(expect_msg.end(), msg.begin(), msg.end());
    expect_msg.push_back(0x42);

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);

    EXPECT_CALL(u_comm, SendMsg(expect_msg))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(excomm.SendCmdPacket(msg));
}

/**
 * @brief 空のコマンドを送る
 *
 */
TEST(ExternalCommunication, SendMsg_4)
{
    std::vector<uint8_t> msg;
    
    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);

    EXPECT_CALL(u_comm, SendMsg(_))
        .Times(0);

    EXPECT_FALSE(excomm.SendCmdPacket(msg));
}

/**
 * @brief 最長+1コマンドを送る
 *
 */
TEST(ExternalCommunication, SendMsg_5)
{
    std::vector<uint8_t> msg(252, 0x00);
    msg[0] = 0x44;
    
    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);

    EXPECT_CALL(u_comm, SendMsg(_))
        .Times(0);

    EXPECT_FALSE(excomm.SendCmdPacket(msg));
}

/**
 * @brief Uartで送信失敗する
 *
 */
TEST(ExternalCommunication, SendMsg_6)
{
    std::vector<uint8_t> msg = {0x44, 0x00};
    std::vector<uint8_t> expect_msg;

    MockExternalCommunicationTask ex_task;
    MockUartCommunication u_comm;

    /* メッセージ作成 */
    expect_msg.push_back(0xFF);
    expect_msg.push_back(0x00);
    expect_msg.push_back(0x06);
    expect_msg.insert(expect_msg.end(), msg.begin(), msg.end());
    expect_msg.push_back(0x49);

    EXPECT_CALL(ex_task, CreateExCommTask())
        .Times(1);

    ExternalCommunication excomm(&u_comm, &ex_task);
    excomm.Init();
    MockCommandMgr cmd(&excomm);

    EXPECT_CALL(u_comm, SendMsg(expect_msg))
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_FALSE(excomm.SendCmdPacket(msg));
}