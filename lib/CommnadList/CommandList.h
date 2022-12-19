/**
 * @file CommandList.h
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief 各コマンドの
 * @version 0.1
 * @date 2022-10-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COMMAND_LIST_H_
#define COMMAND_LIST_H_

#include <stdint.h>
#include <vector>
#include <memory>

#include "CommandBase.h"
#include "IThrusterMgr.h"
#include "IExternalBoard.h"
#include "IHeartBeat.h"

#ifdef _UNIT_TEST
#include <gmock/gmock.h>
#endif

using BoardId = IExternalBoard::BoardId;

/******************************************************************************/
/* CmdError[0x01] *************************************************************/
/******************************************************************************/

/**
 * @brief エラーコマンド
 * 
 */
class CmdError final: public CommandBase
{
public:
    /* エラーの判別コード */
    enum class ErrorType: uint8_t;

    CmdError(ErrorType);
    ~CmdError();

    virtual bool ExcuteCmd(const std::vector<uint8_t>&);
    virtual std::vector<uint8_t> CreateResponse();

private:
    static constexpr uint16_t kCmdArgSize = UINT16_MAX; /* コマンド引数のバイトサイズ   */
    static constexpr uint16_t kResArgSize = 6; /* レスポンス引数のバイトサイズ */
    
    ErrorType err_type_;
};

enum class CmdError::ErrorType: uint8_t
{
    kUndefinedCommand,   /* 未定義コマンドエラー */
    kCheckSumError,      /* Checksumエラー       */
    kPacketSyntaxError,  /* パケット構文エラー   */
    kCommandSyntaxError, /* コマンド構文エラー   */
    kSystemError         /* システム内エラー     */
};

/******************************************************************************/
/* CmdSetBobPower[0x02] *******************************************************/
/******************************************************************************/

class CmdSetBobPower: public CommandBase
{
public:
    CmdSetBobPower(IExternalBoard&);
    ~CmdSetBobPower();
    virtual bool ExcuteCmd(const std::vector<uint8_t>&);
    virtual std::vector<uint8_t> CreateResponse();

private:
    /* 定数宣言 */
    static constexpr uint16_t kCmdArgSize = 1; /* コマンド引数のバイトサイズ   */
    static constexpr uint16_t kResArgSize = 1; /* レスポンス引数のバイトサイズ */
    
    /* 変数宣言 */
    IExternalBoard& ex_board_;
};

/******************************************************************************/
/* CmdSetPrbPower[0x04] *******************************************************/
/******************************************************************************/

class CmdSetPrbPower: public CommandBase
{
public:
    CmdSetPrbPower(IExternalBoard&);
    ~CmdSetPrbPower();
    virtual bool ExcuteCmd(const std::vector<uint8_t>&);
    virtual std::vector<uint8_t> CreateResponse();

private:
    /* 定数宣言 */
    static constexpr uint16_t kCmdArgSize = 2; /* コマンド引数のバイトサイズ   */
    static constexpr uint16_t kResArgSize = 1; /* レスポンス引数のバイトサイズ */
    
    /* 変数宣言 */
    IExternalBoard& ex_board_;
};

/******************************************************************************/
/* CmdSetCommMonitor[0x12] ****************************************************/
/******************************************************************************/
class CmdSetCommMonitor final: public CommandBase
{
public:
    CmdSetCommMonitor(IHeartBeat&);
    ~CmdSetCommMonitor() = default;
    virtual bool ExcuteCmd(const std::vector<uint8_t>&);
    virtual std::vector<uint8_t> CreateResponse();

private:
    /* 定数宣言 */
                                            /* コマンド引数のバイトサイズ     */
    static constexpr uint16_t kCmdArgSize = 3; 
                                            /* レスポンス引数のバイトサイズ   */
    static constexpr uint16_t kResArgSize = 3; 

    /* 型宣言 */
    struct CmdArgList;
    union CmdField;
    struct ResArgList;
    union ResField;

    /* 変数宣言 */
    IHeartBeat& heart_beat_;
    std::vector<uint8_t> response_;
};

#pragma pack(1)
struct CmdSetCommMonitor::CmdArgList
{
    uint8_t monitor_mode;   /* 監視モード 0x00:OFF, 0x01:ON */
    uint16_t timeout;       /* タイムアウト時間[ms] */
};
#pragma pack()

union CmdSetCommMonitor::CmdField
{
    CmdArgList cmd_arg_list;
    uint8_t cmd_arg_byte[kCmdArgSize];
};

#pragma pack(1)
struct CmdSetCommMonitor::ResArgList
{
    uint8_t monitor_mode;   /* 監視モード 0x00:OFF, 0x01:ON */
    uint16_t timeout;       /* タイムアウト時間[ms] */
};
#pragma pack()

union CmdSetCommMonitor::ResField
{
    ResArgList res_arg_list;
    uint8_t res_arg_byte[kResArgSize];
};


/******************************************************************************/
/* CmdControl[0x24] ***********************************************************/
/******************************************************************************/

/**
 * @brief 機体制御コマンド
 *        スラスタを回転、姿勢情報取得
 * 
 */
class CmdControl final: public CommandBase
{
public:

    CmdControl(IThrusterMgr&, IExternalBoard&);
    ~CmdControl();
    virtual bool ExcuteCmd(const std::vector<uint8_t>& cmd_arg);
    virtual std::vector<uint8_t> CreateResponse();

private:
    /* 定数宣言 */
    static constexpr uint16_t kCmdArgSize = 16; /* コマンド引数のバイトサイズ   */
    static constexpr uint16_t kResArgSize = 8; /* レスポンス引数のバイトサイズ */
    static constexpr uint16_t kThrusterNum = 8; /* スラスタ数 */

    /* 型宣言 */
    struct CmdArgList;
    union CmdField;

    /* 変数宣言 */
    std::vector<uint8_t> response_;
    IThrusterMgr& thruster_mgr_;
    IExternalBoard& ex_board_;
};

#pragma pack(1)
struct CmdControl::CmdArgList
{
    uint16_t thruster_power[kThrusterNum];
};
#pragma pack()

union CmdControl::CmdField
{
    CmdArgList cmd_arg_list;
    uint8_t cmd_arg_byte[kCmdArgSize];
};


#endif /* COMMAND_LIST_H_ */