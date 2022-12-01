#ifndef COMMAND_MGR_H_
#define COMMAND_MGR_H_


#include <stdint.h>
#include <gmock/gmock.h>
#include <vector>

#include "CommandMgr.h"

class MockCommandMgr : public CommandMgr
{
public:
    MockCommandMgr(ExternalCommunication* ex_comm) 
    :CommandMgr(ex_comm, NULL)
    {
    }
    MOCK_METHOD1(ParseCmd, bool(std::vector<uint8_t>));
    MOCK_METHOD0(IssueCheckSumErr, bool(void));
    MOCK_METHOD0(IssuePacketSyntaxErr, bool(void));
	/* メンバ関数宣言 */
	static void DetectRecvCmdEntryFunc(void*, std::vector<uint8_t>);
	static void DetectPacketSyntaxErrEntryFunc(void*);
	static void DetectCheckSumErrEntryFunc(void*);

};

/**
 * @brief コマンド取得時に呼ばれる関数
 * 
 * @param callback_instance 呼ばれるインスタンス
 * @param cmd_msg           取得したコマンド
 */
void MockCommandMgr::DetectRecvCmdEntryFunc(void* callback_instance, 
                                        std::vector<uint8_t> cmd_msg)
{
    reinterpret_cast<MockCommandMgr*>(callback_instance)->ParseCmd(cmd_msg);
}

/**
 * @brief パケット構文エラー発生時に呼ばれるコールバック関数
 * 
 * @param callback_instance 
 */
void MockCommandMgr::DetectPacketSyntaxErrEntryFunc(void* callback_instance)
{
    reinterpret_cast<MockCommandMgr*>(callback_instance)->IssuePacketSyntaxErr();
}

/**
 * @brief CheckSumエラー発生時に呼ばれるコールバック関数
 * 
 * @param callback_instance 
 */
void MockCommandMgr::DetectCheckSumErrEntryFunc(void* callback_instance)
{
    reinterpret_cast<MockCommandMgr*>(callback_instance)->IssueCheckSumErr();
}


#endif /* COMMAND_MGR_H_ */