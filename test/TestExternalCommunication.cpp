#include "gtest/gtest.h"
#include <gmock/gmock.h>

#include <MockFreeRtos.h>

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

extern freertos::MockFreeRtos MockFreeRtosObj;

void CallPerformTask(std::shared_ptr<ExternalCommunication> ex_comm)
{
    ex_comm->ParsePacket();
}

class ExternalCommunicationTestFixture: public::testing::Test
{
protected:
    std::shared_ptr<ExternalCommunication> ex_comm_;
    std::shared_ptr<MockUartCommunication> uart_comm_;
    std::shared_ptr<MockCommandMgr> cmd_mgr_;

    virtual void SetUp()
    {
        uart_comm_ = std::make_shared<MockUartCommunication>();
        ex_comm_ = std::make_shared<ExternalCommunication>(uart_comm_);
        cmd_mgr_ = std::make_shared<MockCommandMgr>(ex_comm_);

        EXPECT_CALL(MockFreeRtosObj, xTaskCreate(_, _, _, _, _, _))
            .Times(1);
        
        ex_comm_->Init();
    }

    virtual void TearDown()
    {
    }
};

/**
 * @brief コマンド値のみ
 *        受信通知コールバックが呼ばれること
 *
 */
TEST_F(ExternalCommunicationTestFixture, ParsePacket_2)
{
    std::vector<uint8_t> msg = {0xFF, 0x00, 0x05, 0x55, 0x59};
    uint16_t cnt = 0;
    std::vector<uint8_t> recv_cmd = {0x55};


    ex_comm_->RegistNotifyCallback(
        cmd_mgr_,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);
    EXPECT_CALL(*uart_comm_, WaitReceiveData(portMAX_DELAY))
        .Times(msg.size());
    EXPECT_CALL(*cmd_mgr_, ParseCmd(recv_cmd))
        .Times(1);
    EXPECT_CALL(*cmd_mgr_, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(*cmd_mgr_, IssueCheckSumErr())
        .Times(0);

    EXPECT_CALL(*uart_comm_, ReadByte())
        .Times(msg.size())
        .WillOnce(Return(msg[cnt++]))
        .WillOnce(Return(msg[cnt++]))
        .WillOnce(Return(msg[cnt++]))
        .WillOnce(Return(msg[cnt++]))
        .WillOnce(Return(msg[cnt++]));

    for (uint16_t i = 0; i < msg.size(); i++)
    {
        CallPerformTask(ex_comm_);
    }
    
}

/**
 * @brief 引数1個のコマンド
 *        受信通知コールバックが呼ばれること
 *
 */

TEST_F(ExternalCommunicationTestFixture, ParsePacket_3)
{
    uint16_t msg_len = 6;
    std::vector<uint8_t> msg;
    uint8_t msg_cnt = 0;
    uint16_t cnt = 0;
    std::vector<uint8_t> recv_cmd;

    /* メッセージ作成 */
    msg = {0xFF, 0x00, static_cast<uint8_t>(msg_len)};
    for (uint16_t i = msg.size(); i < msg_len - 1; i++)
    {
        msg.push_back(0x00);
        recv_cmd.push_back(0x00);
    }
    msg.push_back(msg[2] - 1);

    ex_comm_->RegistNotifyCallback(
        cmd_mgr_,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(*uart_comm_, WaitReceiveData(portMAX_DELAY))
        .Times(msg.size());

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(*uart_comm_, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(*cmd_mgr_, ParseCmd(recv_cmd))
        .Times(1);
    EXPECT_CALL(*cmd_mgr_, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(*cmd_mgr_, IssueCheckSumErr())
        .Times(0);

    for (uint16_t i = 0; i < msg.size(); i++)
    {
        CallPerformTask(ex_comm_);
    }
    
}

/**
 * @brief 最長コマンド
 *        
 */

TEST_F(ExternalCommunicationTestFixture, ParsePacket_4)
{
    uint16_t msg_len = 255;
    std::vector<uint8_t> msg;
    std::vector<uint8_t> expect_cmd(msg_len - 4, 0x00);

    msg.push_back(0xFF);
    msg.push_back(0x00);
    msg.push_back(msg_len);
    msg.insert(msg.end(), expect_cmd.begin(), expect_cmd.end());
    msg.push_back(msg[2] - 1);

    ex_comm_->RegistNotifyCallback(
        cmd_mgr_,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(*uart_comm_, WaitReceiveData(portMAX_DELAY))
        .Times(msg.size());
    EXPECT_CALL(*cmd_mgr_, ParseCmd(expect_cmd))
        .Times(1);
    EXPECT_CALL(*cmd_mgr_, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(*cmd_mgr_, IssueCheckSumErr())
        .Times(0);

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(*uart_comm_, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    for (uint16_t i = 0; i < msg.size(); i++)
    {
        CallPerformTask(ex_comm_);
    }
}

/**
 * @brief パケット文法エラーコマンドを取得する
 *        SyntaxErrコールバック関数が呼ばれること
 *
 */
TEST_F(ExternalCommunicationTestFixture, ParsePacket_5)
{
    // InSequence s;
    uint16_t msg_len = 6;
    std::vector<uint8_t> msg;
    uint8_t msg_cnt = 0;
    uint16_t cnt = 0;
    std::vector<uint8_t> recv_cmd;

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

    ex_comm_->RegistNotifyCallback(
        cmd_mgr_,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(*uart_comm_, WaitReceiveData(portMAX_DELAY))
        .Times(msg.size());

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(*uart_comm_, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(*cmd_mgr_, ParseCmd(_))
        .Times(0);
    EXPECT_CALL(*cmd_mgr_, IssuePacketSyntaxErr())
        .Times(1);
    EXPECT_CALL(*cmd_mgr_, IssueCheckSumErr())
        .Times(0);

    for (uint16_t i = 0; i < msg.size(); i++)
    {
        CallPerformTask(ex_comm_);
    }
}

/**
 * @brief チェックサムエラーコマンドを取得する
 *        checksum通知コールバックが呼ばれること
 *
 */
TEST_F(ExternalCommunicationTestFixture, ParsePacket_6)
{
    // InSequence s;
    uint16_t msg_len = 6;
    std::vector<uint8_t> msg;
    uint8_t msg_cnt = 0;
    uint16_t cnt = 0;
    std::vector<uint8_t> recv_cmd;

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

    ex_comm_->RegistNotifyCallback(
        cmd_mgr_,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(*uart_comm_, WaitReceiveData(portMAX_DELAY))
        .Times(msg.size());

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(*uart_comm_, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(*cmd_mgr_, ParseCmd(_))
        .Times(0);
    EXPECT_CALL(*cmd_mgr_, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(*cmd_mgr_, IssueCheckSumErr())
        .Times(1);

    for (uint16_t i = 0; i < msg.size(); i++)
    {
        CallPerformTask(ex_comm_);
    }
}

/**
 * @brief パケット始端コード以外を取得させる
 *
 */
TEST_F(ExternalCommunicationTestFixture, ParsePacket_7)
{
    // InSequence s;
    uint16_t msg_len = 6;
    std::vector<uint8_t> msg = {0x55, 0x44, 0x33}; /* 最初にゴミを入れておく */
    std::vector<uint8_t> recv_cmd;

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

    ex_comm_->RegistNotifyCallback(
        cmd_mgr_,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);

    EXPECT_CALL(*uart_comm_, WaitReceiveData(portMAX_DELAY))
        .Times(msg.size());

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(*uart_comm_, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(*cmd_mgr_, ParseCmd(recv_cmd))
        .Times(1);
    EXPECT_CALL(*cmd_mgr_, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(*cmd_mgr_, IssueCheckSumErr())
        .Times(0);

    for (uint16_t i = 0; i < msg.size(); i++)
    {
        CallPerformTask(ex_comm_);
    }
}

/**
 * @brief パケット始端コード2以外を取得させ、その後パケットを取得する
 *        正常なパケットのみ取得できること
 *
 */
TEST_F(ExternalCommunicationTestFixture, ParsePacket_8)
{
    uint16_t msg_len = 6;
    std::vector<uint8_t> invalid_msg = {0xFF, 0x55, 0xFF, 0x44};
    std::vector<uint8_t> msg;
    std::vector<uint8_t> expect_cmd;

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
    
    ex_comm_->RegistNotifyCallback(
        cmd_mgr_,
        MockCommandMgr::DetectRecvCmdEntryFunc,
        MockCommandMgr::DetectPacketSyntaxErrEntryFunc,
        MockCommandMgr::DetectCheckSumErrEntryFunc);
        
    EXPECT_CALL(*uart_comm_, WaitReceiveData(portMAX_DELAY))
        .Times(msg.size());

    for (int16_t i = msg.size() - 1; i >= 0; i--)
    {
        EXPECT_CALL(*uart_comm_, ReadByte())
            .WillOnce(Return(msg[i]))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(*cmd_mgr_, ParseCmd(expect_cmd))
        .Times(1);
    EXPECT_CALL(*cmd_mgr_, IssuePacketSyntaxErr())
        .Times(0);
    EXPECT_CALL(*cmd_mgr_, IssueCheckSumErr())
        .Times(0);

    for (uint16_t i = 0; i < msg.size(); i++)
    {
        CallPerformTask(ex_comm_);
    }
}

/******************************************************************************/
/* SendMsgテスト     **********************************************************/
/******************************************************************************/

/**
 * @brief コマンド値のみ送信
 *
 */
TEST_F(ExternalCommunicationTestFixture, SendMsg_1)
{
    std::vector<uint8_t> msg = {0x44};
    std::vector<uint8_t> expect_msg;
    

    /* メッセージ作成 */
    expect_msg.push_back(0xFF);
    expect_msg.push_back(0x00);
    expect_msg.push_back(0x05);
    expect_msg.insert(expect_msg.end(), msg.begin(), msg.end());
    expect_msg.push_back(0x48);

    EXPECT_CALL(*uart_comm_, SendMsg(expect_msg))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(ex_comm_->SendCmdPacket(msg));
}

/**
 * @brief 引数1のコマンドを送る
 *
 */
TEST_F(ExternalCommunicationTestFixture, SendMsg_2)
{
    std::vector<uint8_t> msg = {0x44, 0x00};
    std::vector<uint8_t> expect_msg;
    

    /* メッセージ作成 */
    expect_msg.push_back(0xFF);
    expect_msg.push_back(0x00);
    expect_msg.push_back(0x06);
    expect_msg.insert(expect_msg.end(), msg.begin(), msg.end());
    expect_msg.push_back(0x49);

    EXPECT_CALL(*uart_comm_, SendMsg(expect_msg))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(ex_comm_->SendCmdPacket(msg));
}

/**
 * @brief 最長コマンドを送る
 *
 */
TEST_F(ExternalCommunicationTestFixture, SendMsg_3)
{
    std::vector<uint8_t> msg(251, 0x00);
    msg[0] = 0x44;
    std::vector<uint8_t> expect_msg;
    

    /* メッセージ作成 */
    expect_msg.push_back(0xFF);
    expect_msg.push_back(0x00);
    expect_msg.push_back(0xFF);
    expect_msg.insert(expect_msg.end(), msg.begin(), msg.end());
    expect_msg.push_back(0x42);

    EXPECT_CALL(*uart_comm_, SendMsg(expect_msg))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(ex_comm_->SendCmdPacket(msg));
}

/**
 * @brief 空のコマンドを送る
 *
 */
TEST_F(ExternalCommunicationTestFixture, SendMsg_4)
{
    std::vector<uint8_t> msg;
    
    EXPECT_CALL(*uart_comm_, SendMsg(_))
        .Times(0);

    EXPECT_FALSE(ex_comm_->SendCmdPacket(msg));
}

/**
 * @brief 最長+1コマンドを送る
 *
 */
TEST_F(ExternalCommunicationTestFixture, SendMsg_5)
{
    std::vector<uint8_t> msg(252, 0x00);
    msg[0] = 0x44;

    EXPECT_CALL(*uart_comm_, SendMsg(_))
        .Times(0);

    EXPECT_FALSE(ex_comm_->SendCmdPacket(msg));
}

/**
 * @brief Uartで送信失敗する
 *
 */
TEST_F(ExternalCommunicationTestFixture, SendMsg_6)
{
    std::vector<uint8_t> msg = {0x44, 0x00};
    std::vector<uint8_t> expect_msg;

    /* メッセージ作成 */
    expect_msg.push_back(0xFF);
    expect_msg.push_back(0x00);
    expect_msg.push_back(0x06);
    expect_msg.insert(expect_msg.end(), msg.begin(), msg.end());
    expect_msg.push_back(0x49);

    EXPECT_CALL(*uart_comm_, SendMsg(expect_msg))
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_FALSE(ex_comm_->SendCmdPacket(msg));
}