#ifndef COMMAND_BASE_H_
#define COMMAND_BASE_H_

#include <stdint.h>
#include <vector>
#include <string.h>
#include <memory>

class CommandBase
{
public:
    CommandBase() = delete; /* コンストラクタの定義を強制 */
    CommandBase(const uint16_t);

    virtual ~CommandBase();

    virtual bool CheckCmdLen(uint16_t) final;
    virtual bool ExcuteCmd(const std::vector<uint8_t>&) = 0;
    virtual std::vector<uint8_t> CreateResponse() = 0;
protected:
    const uint16_t arg_len_;
    std::vector<uint8_t> response_;
};



#endif /* COMMAND_BASE_H_ */