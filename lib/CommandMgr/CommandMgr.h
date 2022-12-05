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

class CommandMgr: public std::enable_shared_from_this<CommandMgr>
{
public:
	CommandMgr(	const std::shared_ptr<IExternalCommunication>,
				const std::shared_ptr<IThrusterMgr>,
				const std::shared_ptr<IExternalBoard>,
				const std::shared_ptr<IBmsMgr>,
				const std::shared_ptr<IHeartBeat>);
	virtual ~CommandMgr();

	bool Init();

private:

	enum class CmdValue;
	/* 定数宣言 */
	static constexpr uint16_t kPacketMaxByteSize = 256;
	static constexpr uint16_t kUartBufferSize = 64;
	static constexpr uint16_t kMsgBufferSize = 128;

	/* メンバ変数宣言 */
	const std::shared_ptr<IExternalCommunication> ex_comm_;
	const std::shared_ptr<IThrusterMgr> thruster_mgr_;
	const std::shared_ptr<IExternalBoard> ex_board_;
	const std::shared_ptr<IBmsMgr> bms_mgr_;
	const std::shared_ptr<IHeartBeat> heart_beat_;

	/* メンバ関数宣言 */
	static void DetectRecvCmdEntryFunc(std::shared_ptr<void>, 
										std::vector<uint8_t>);
	static void DetectPacketSyntaxErrEntryFunc(std::shared_ptr<void>);
	static void DetectCheckSumErrEntryFunc(std::shared_ptr<void>);
	bool ParseCmd(std::vector<uint8_t>);
	bool IssueCheckSumErr();
	bool IssuePacketSyntaxErr();
	bool IssueUndefinedCmdErr();
	bool IssueCmdSyntaxErr();
	bool IssueSystemErr();
	std::shared_ptr<CommandBase> MakeCmdInstance(uint8_t);

public:
	/* コマンド値定義 */
	static constexpr uint8_t kCmdError       = 0x01;
	static constexpr uint8_t kCmdSetBobPower = 0x02;
	static constexpr uint8_t kCmdSetPrbPower = 0x04;
	static constexpr uint8_t kCmdSetEobPower = 0x06;
	static constexpr uint8_t kCmdControl     = 0x18;
};

#endif /* SRC_COMMAND_MGR_H_ */