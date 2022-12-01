#include "CommandBase.h"

CommandBase::CommandBase(const uint16_t cmd_len)
:cmd_len_(cmd_len)
{
}

CommandBase::~CommandBase()
{

}

/**
 * @brief コマンドのバイトサイズを比較する
 * 
 * @param cmd_len チェック対象のコマンド長
 * @return bool true:一致 false:不一致
 */
bool CommandBase::CheckCmdLen(uint16_t cmd_len)
{
    if (cmd_len_ != cmd_len)
    {
        return false;
    }
    return true;
}
