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

#ifdef _UNIT_TEST
#include <gmock/gmock.h>
#endif

using BoardId = IExternalBoard::BoardId;

/******************************************************************************/
/* CmdError *******************************************************************/
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

    virtual bool ExcuteCmd(std::vector<uint8_t>);
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
/* CmdSetBobPower *************************************************************/
/******************************************************************************/

class CmdSetBobPower: public CommandBase
{
public:
    CmdSetBobPower(const std::shared_ptr<IExternalBoard>);
    ~CmdSetBobPower();
    virtual bool ExcuteCmd(std::vector<uint8_t>);
    virtual std::vector<uint8_t> CreateResponse();

private:
    /* 定数宣言 */
    static constexpr uint16_t kCmdArgSize = 1; /* コマンド引数のバイトサイズ   */
    static constexpr uint16_t kResArgSize = 1; /* レスポンス引数のバイトサイズ */
    
    /* 変数宣言 */
    const std::shared_ptr<IExternalBoard> ex_board_;
};

/******************************************************************************/
/* CmdSetPrbPower *************************************************************/
/******************************************************************************/

class CmdSetPrbPower: public CommandBase
{
public:
    CmdSetPrbPower(const std::shared_ptr<IExternalBoard>);
    ~CmdSetPrbPower();
    virtual bool ExcuteCmd(std::vector<uint8_t>);
    virtual std::vector<uint8_t> CreateResponse();

private:
    /* 定数宣言 */
    static constexpr uint16_t kCmdArgSize = 2; /* コマンド引数のバイトサイズ   */
    static constexpr uint16_t kResArgSize = 1; /* レスポンス引数のバイトサイズ */
    
    /* 変数宣言 */
    const std::shared_ptr<IExternalBoard> ex_board_;
};

/******************************************************************************/
/* CmdControl *****************************************************************/
/******************************************************************************/

/**
 * @brief 機体制御コマンド
 *        スラスタを回転、姿勢情報取得
 * 
 */
class CmdControl final: public CommandBase
{
public:

    CmdControl(const std::shared_ptr<IThrusterMgr>, const std::shared_ptr<IExternalBoard>);
    ~CmdControl();
    virtual bool ExcuteCmd(const std::vector<uint8_t> cmd_arg);
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
    const std::shared_ptr<IThrusterMgr> thruster_mgr_;
    const std::shared_ptr<IExternalBoard> ex_board_;
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