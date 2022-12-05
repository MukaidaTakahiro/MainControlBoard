#ifndef I_THRUSTER_MGR_H_
#define I_THRUSTER_MGR_H_

#include <stdint.h>
#include <vector>

class IThrusterMgr
{
public:
    virtual bool OperateThruster(const std::vector<uint16_t>) = 0;
};


#endif /* I_THRUSTER_MGR_H_ */
