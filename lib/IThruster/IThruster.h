#ifndef I_THRUSTER_H_
#define I_THRUSTER_H_

#include <stdint.h>
#include <vector>
#include <memory>

class IThruster
{
public:
    virtual bool SetPulseWidth(uint32_t) = 0;
};


#endif /* I_THRUSTER_H_ */
