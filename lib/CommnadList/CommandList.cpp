#include "CommandList.h"

/******************************************************************************/
/* CmdError *******************************************************************/
/******************************************************************************/
CmdError::CmdError(ErrorType err_type)
:CommandBase(kCmdArgSize),err_type_(err_type)
{}

CmdError::~CmdError()
{}

bool CmdError::ExcuteCmd(std::vector<uint8_t> cmd_arg)
{
    return false;
}

std::vector<uint8_t> CmdError::CreateResponse()
{
    std::vector<uint8_t> res_arg(1, static_cast<uint8_t>(err_type_));
    return res_arg;
}

/******************************************************************************/
/* CmdSetBobPower *************************************************************/
/******************************************************************************/
CmdSetBobPower::CmdSetBobPower(const std::shared_ptr<IExternalBoard> ex_board)
:   CommandBase(kCmdArgSize), ex_board_(ex_board)
{}

CmdSetBobPower::~CmdSetBobPower()
{}

bool CmdSetBobPower::ExcuteCmd(std::vector<uint8_t> cmd_arg)
{
    std::vector<uint8_t> response;
    
    /* コマンド送信 */
    if(!ex_board_->SendCmd(BoardId::kBob, cmd_arg, &response))
    {
        return false;
    }

    /* 受信処理 */
    response_.clear();
    response_.insert(response_.begin(), response.begin(), response.end());
    
    return false;
}

std::vector<uint8_t> CmdSetBobPower::CreateResponse()
{
    return response_;
}

/******************************************************************************/
/* CmdSetPrbPower *************************************************************/
/******************************************************************************/
CmdSetPrbPower::CmdSetPrbPower(const std::shared_ptr<IExternalBoard> ex_board)
:   CommandBase(kCmdArgSize), ex_board_(ex_board)
{}

CmdSetPrbPower::~CmdSetPrbPower()
{}

bool CmdSetPrbPower::ExcuteCmd(std::vector<uint8_t> cmd_arg)
{
    std::vector<uint8_t> response;
    
    /* コマンド送信 */
    if(!ex_board_->SendCmd(BoardId::kPrb, cmd_arg, &response))
    {
        return false;
    }

    /* 受信処理 */
    response_.clear();
    response_.insert(response_.begin(), response.begin(), response.end());
    
    return true;
}

std::vector<uint8_t> CmdSetPrbPower::CreateResponse()
{
    return response_;
}

/******************************************************************************/
/* CmdSetCommMonitor **********************************************************/
/******************************************************************************/
CmdSetCommMonitor::CmdSetCommMonitor(IHeartBeat& heart_beat)
:   CommandBase(kCmdArgSize), heart_beat_(heart_beat)
{
    response_.reserve(kResArgSize);
}

bool CmdSetCommMonitor::ExcuteCmd(const std::vector<uint8_t> cmd_msg)
{
    std::vector<uint8_t> cmd_arg {cmd_msg.begin() + 1, cmd_msg.end()};
    CmdField cmd_field;

    memcpy(cmd_field.cmd_arg_byte, cmd_arg.data(), cmd_arg.size());

    if (cmd_field.cmd_arg_list.monitor_mode == 0)
    {
        heart_beat_.StopMonitoring();
    }
    else if (cmd_field.cmd_arg_list.monitor_mode == 1)
    {
        heart_beat_.SetMonitoringTimeout(cmd_field.cmd_arg_list.timeout);
        heart_beat_.StartMonitoring();
    }
    
    ResField res_field;
    res_field.res_arg_list.monitor_mode = heart_beat_.GetTimeout() ? 1 : 0;
    res_field.res_arg_list.timeout = heart_beat_.GetTimeout();

    response_.clear();
    response_.insert(   response_.begin(), 
                        res_field.res_arg_byte, 
                        res_field.res_arg_byte + kResArgSize);

    return true;
}


/******************************************************************************/
/* CmdControl *****************************************************************/
/******************************************************************************/

CmdControl::CmdControl( const std::shared_ptr<IThrusterMgr> thruster, 
                        const std::shared_ptr<IExternalBoard> ex_board)
:   CommandBase(kCmdArgSize),  thruster_mgr_(thruster), ex_board_(ex_board)
{
    response_.reserve(kResArgSize);
}

CmdControl::~CmdControl()
{
}

bool CmdControl::ExcuteCmd(const std::vector<uint8_t> cmd_msg)
{
    /* 各引数に変換 */
    std::vector<uint8_t> cmd_arg {cmd_msg.begin() + 1, cmd_msg.end()};
    CmdField cmd_field;
    std::vector<uint16_t> thruster_power_vec;

    memcpy(cmd_field.cmd_arg_byte, cmd_arg.data(), cmd_arg.size());

    /* スラスタ出力値をvecに変換 */
    thruster_power_vec.insert(
        thruster_power_vec.begin(),
        std::begin(cmd_field.cmd_arg_list.thruster_power),
        std::end(cmd_field.cmd_arg_list.thruster_power));

    /* スラスタ値を出力 */
    if(!thruster_mgr_->OperateThruster(thruster_power_vec))
    {
        return false;
    }

    /* 姿勢データを取得 */
    uint8_t cmd_value = cmd_msg.front();
    std::vector<uint8_t> send_cmd;
    send_cmd.push_back(cmd_value);

    std::vector<uint8_t> response;

#ifndef _NONE_BOB    
    if (!ex_board_->SendCmd(BoardId::kBob, send_cmd, &response))
    {
        return false;
    }
#else
    response.push_back(cmd_value + 1);
    for (uint16_t i = 0; i < kResArgSize; i++)
    {
        response.push_back(i);
    }
#endif
    
    // 返信をレスポンスを作る
    response_.clear();
    std::copy(response.begin(), response.end(), std::back_inserter(response_));

    return true;
}

std::vector<uint8_t> CmdControl::CreateResponse()
{
    return response_;
}
