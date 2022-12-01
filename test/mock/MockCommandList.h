#ifndef MOCK_COMMAND_LIST_H_
#define MOCK_COMMAND_LIST_H_

#include <vector>
#include <gmock/gmock.h>

#include "CommandBase.h"


/**
 * @brief エラーコマンド
 * 
 */
class CmdError: public CommandBase
{
public:
    /* エラーの判別コード */
    enum class ErrorType : uint8_t
    {
        kUndefinedCommand,  /* 未定義コマンドエラー */
        kCheckSumError,     /* Checksumエラー       */
        kPacketSyntaxError, /* パケット構文エラー   */
        kCommandSyntaxError,/* コマンド構文エラー   */
        kSystemError        /* システム内エラー     */
    };

    CmdError(ErrorType err_type)
    :CommandBase(kCmdArgSize, NULL),err_type_(err_type)
    {}
    ~CmdError()
    {}

    MOCK_METHOD1(ExcuteCmd, bool(std::vector<uint8_t>));
    MOCK_METHOD0(CreateResponse, std::vector<uint8_t>(void));

    static constexpr uint16_t kCmdArgSize = UINT16_MAX; /* コマンド引数のバイトサイズ   */
    ErrorType err_type_;
};


/**
 * @brief 高優先度コマンド
 * 
 */
class CmdControl final: public CommandBase
{
public:

    CmdControl(IDevice* device) 
    :CommandBase(kCmdArgSize, device)
    {}
    ~CmdControl()
    {}

    MOCK_METHOD1(ExcuteCmd, bool(std::vector<uint8_t>));
    MOCK_METHOD0(CreateResponse, std::vector<uint8_t>(void));

    static constexpr uint16_t kCmdArgSize = 16; /* コマンド引数のバイトサイズ   */
};

#endif /* MOCK_COMMAND_LIST_H_ */