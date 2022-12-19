#include "CommandMgr.h"

#ifdef _UNIT_TEST
#include <gmock/gmock.h>
#endif

/* コマンド値定義 */
constexpr uint8_t CommandMgr::kCmdError;
constexpr uint8_t CommandMgr::kCmdSetBobPower;
constexpr uint8_t CommandMgr::kCmdSetPrbPower;
constexpr uint8_t CommandMgr::kCmdSetEobPower;
constexpr uint8_t CommandMgr::kCmdControl;

constexpr uint16_t CommandMgr::kPacketMaxByteSize;
constexpr uint16_t CommandMgr::kUartBufferSize;
constexpr uint16_t CommandMgr::kMsgBufferSize;

CommandMgr::CommandMgr(	IExternalCommunication& ex_comm,
                        IThrusterMgr& thruster,
                        IExternalBoard& ex_board,
                        IBmsMgr& bms_mgr,
                        IHeartBeat& heart_beat)
:   ex_comm_(ex_comm), 
    thruster_mgr_(thruster), 
    ex_board_(ex_board), 
    bms_mgr_(bms_mgr), 
    heart_beat_(heart_beat)
{
}

CommandMgr::~CommandMgr()
{
}

bool CommandMgr::Init()
{
    bool result;

    result = ex_comm_.RegistNotifyCallback(this, 
                                            DetectRecvCmdEntryFunc,
                                            DetectPacketSyntaxErrEntryFunc,
                                            DetectCheckSumErrEntryFunc);

    return result;
}


/**
 * @brief コマンド取得時に呼ばれる関数
 * 
 * @param callback_instance 呼ばれるインスタンス
 * @param cmd_msg           取得したコマンド
 */
void CommandMgr::DetectRecvCmdEntryFunc(void* callback_instance, 
                                        const std::vector<uint8_t>& cmd_msg)
{
    reinterpret_cast<CommandMgr*>(callback_instance)->ParseCmd(cmd_msg);
}

/**
 * @brief 取得したコマンドを解析する
 * 
 * @param cmd_msg 取得したコマンド
 * @return bool 
 */
bool CommandMgr::ParseCmd(const std::vector<uint8_t>& cmd_msg)
{
    uint8_t cmd_value = cmd_msg.front();
    auto cmd = MakeCmdInstance(cmd_value);

    if (cmd == nullptr)
    {
        IssueUndefinedCmdErr();
        return false;
    }
    
    /* コマンド長チェック */
    if (!cmd->CheckCmdLen(cmd_msg.size() - 1))
    {
        IssueCmdSyntaxErr();
        return false;
    }
    
    /* 命令実行 */
    if (!cmd->ExcuteCmd(cmd_msg))
    {
        IssueSystemErr();
        return false;
    }

    /* 応答コマンド取得 */
    std::vector<uint8_t> res_msg;
    res_msg = cmd->CreateResponse();

    if (res_msg.empty())
    {
        /* システムエラー発行 */
        IssueSystemErr();
        return false;
    }

    
    /* 応答コマンド送信 */
    ex_comm_.SendCmdPacket(res_msg);
    
    return true;
}

/**
 * @brief パケット構文エラー発生時に呼ばれるコールバック関数
 * 
 * @param callback_instance 
 */
void CommandMgr::DetectPacketSyntaxErrEntryFunc(
                    void* callback_instance)
{
    reinterpret_cast<CommandMgr*>(callback_instance)
                                                    ->IssuePacketSyntaxErr();
}



/**
 * @brief パケット文法エラー発生時の処理
 * 
 */
bool CommandMgr::IssuePacketSyntaxErr()
{
    std::vector<uint8_t> res_msg;
    CmdError err_cmd(CmdError::ErrorType::kPacketSyntaxError);

    res_msg = err_cmd.CreateResponse();
    res_msg.insert(res_msg.begin(), kCmdError);

    return ex_comm_.SendCmdPacket(res_msg);
}


/**
 * @brief CheckSumエラー発生時に呼ばれるコールバック関数
 * 
 * @param callback_instance 
 */
void CommandMgr::DetectCheckSumErrEntryFunc(void* callback_instance)
{
    reinterpret_cast<CommandMgr*>(callback_instance)->IssueCheckSumErr();
}


/**
 * @brief チェックサムエラー発生時の処理
 * 
 * @return bool 送信成否
 */
bool CommandMgr::IssueCheckSumErr()
{
    std::vector<uint8_t> res_msg;
    CmdError err_cmd(CmdError::ErrorType::kCheckSumError);
    
    res_msg = err_cmd.CreateResponse();
    res_msg.insert(res_msg.begin(), kCmdError);

    return ex_comm_.SendCmdPacket(res_msg);
}

/**
 * @brief 未定義コマンドエラー発生時の処理
 * 
 * @return bool 送信成否
 */
bool CommandMgr::IssueUndefinedCmdErr()
{
    std::vector<uint8_t> res_msg;
    CmdError err_cmd(CmdError::ErrorType::kUndefinedCommand);
    
    res_msg = err_cmd.CreateResponse();
    res_msg.insert(res_msg.begin(), kCmdError);

    return ex_comm_.SendCmdPacket(res_msg);
}

/**
 * @brief コマンド構文エラー発報
 * 
 * @return bool 送信成否
 */
bool CommandMgr::IssueCmdSyntaxErr()
{    
    std::vector<uint8_t> res_msg;
    CmdError err_cmd(CmdError::ErrorType::kCommandSyntaxError);
    
    res_msg = err_cmd.CreateResponse();
    res_msg.insert(res_msg.begin(), kCmdError);
    
    return ex_comm_.SendCmdPacket(res_msg);
}

/**
 * @brief システムエラー発報
 * 
 * @return bool 送信成否
 */
bool CommandMgr::IssueSystemErr()
{
    std::vector<uint8_t> res_msg;
    CmdError err_cmd(CmdError::ErrorType::kSystemError);
    
    res_msg = err_cmd.CreateResponse();
    res_msg.insert(res_msg.begin(), kCmdError);

    return ex_comm_.SendCmdPacket(res_msg);
}

/**
 * @brief コマンドインスタンス生成
 * 
 * @param cmd_value コマンド値
 * @return std::unique_ptr<CommandBase> 
 */
std::unique_ptr<CommandBase> 
    CommandMgr::MakeCmdInstance(const uint8_t cmd_value)
{
    std::unique_ptr<CommandBase> instance = nullptr;

    switch (cmd_value)
    {
    case kCmdSetPrbPower:
        instance = std::make_unique<CmdSetPrbPower>(ex_board_);
        break;

    case kCmdSetCommMonitor:
        instance = std::make_unique<CmdSetCommMonitor>(heart_beat_);
        break;

    case kCmdControl:
        instance = std::make_unique<CmdControl>(thruster_mgr_, ex_board_);
        break;
    
    }

    return std::move(instance);
}