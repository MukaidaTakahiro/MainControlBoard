#include "CommandBase.h"

CommandBase::CommandBase(const uint16_t arg_len)
:arg_len_(arg_len)
{
}

CommandBase::~CommandBase()
{
}

/**
 * @brief コマンドのバイトサイズを比較する
 * 
 * @param arg_len チェック対象のコマンド長
 * @return bool true:一致 false:不一致
 */
bool CommandBase::CheckCmdLen(const uint16_t arg_len)
{
    if (arg_len_ != arg_len)
    {
        return false;
    }
    return true;
}
