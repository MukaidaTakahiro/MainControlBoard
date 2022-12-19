#ifndef SRC_COMMAND_MGR_H_
#define SRC_COMMAND_MGR_H_


#include "IExternalCommunication.h"
#include <stdint.h>
#include <vector>
#include <memory>

#include "CommandBase.h"
#include "IThrusterMgr.h"
#include "IExternalBoard.h"
#include "IBmsMgr.h"
#include "IHeartBeat.h"

#include "CommandList.h"

class CommandMgr
{
public:
	CommandMgr(	IExternalCommunication&,
				IThrusterMgr&,
				IExternalBoard&,
				IBmsMgr&,
				IHeartBeat&);
	virtual ~CommandMgr();

	bool Init();

private:

	enum class CmdValue;
	/* 定数宣言 */
	static constexpr uint16_t kPacketMaxByteSize = 256;
	static constexpr uint16_t kUartBufferSize = 64;
	static constexpr uint16_t kMsgBufferSize = 128;

	/* メンバ変数宣言 */
	IExternalCommunication& ex_comm_;
	IThrusterMgr& thruster_mgr_;
	IExternalBoard& ex_board_;
	IBmsMgr& bms_mgr_;
	IHeartBeat& heart_beat_;

	/* メンバ関数宣言 */
	static void DetectRecvCmdEntryFunc(void*, const std::vector<uint8_t>&);
	static void DetectPacketSyntaxErrEntryFunc(void*);
	static void DetectCheckSumErrEntryFunc(void*);
	bool ParseCmd(const std::vector<uint8_t>&);
	bool IssueCheckSumErr();
	bool IssuePacketSyntaxErr();
	bool IssueUndefinedCmdErr();
	bool IssueCmdSyntaxErr();
	bool IssueSystemErr();
	std::unique_ptr<CommandBase> MakeCmdInstance(uint8_t);

public:
    /* コマンド値定義 */
    static constexpr uint8_t kCmdError          = 0x01;
    static constexpr uint8_t kCmdSetBobPower    = 0x02;
    static constexpr uint8_t kCmdSetPrbPower    = 0x04;
    static constexpr uint8_t kCmdSetEobPower    = 0x06;
    static constexpr uint8_t kCmdGetFwVer       = 0x10;
    static constexpr uint8_t kCmdSetCommMonitor = 0x12;
    static constexpr uint8_t kCmdGetCommMonitor = 0x14;
    static constexpr uint8_t kCmdControl        = 0x18;
};

#endif /* SRC_COMMAND_MGR_H_ */