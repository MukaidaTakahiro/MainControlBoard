#ifndef I_INTERNAL_COMMUNICATION_H_
#define I_INTERNAL_COMMUNICATION_H_

#include <functional>
#include <stdint.h>
#include <memory>
#include <vector>


class IInternalCommunication
{
public:
    virtual bool SendInCmdPacket(   
                            const std::vector<uint8_t>,
                            std::vector<uint8_t>&) = 0;
};

#endif /* I_INTERNAL_COMMUNICATION_H_ */
