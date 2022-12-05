#ifndef THRUSTER_MGR_H_
#define THRUSTER_MGR_H_

#include <stdint.h>
#include <memory.h>
#include <vector>

#include "IThrusterMgr.h"
#include "IThruster.h"

class ThrusterMgr: public IThrusterMgr
{
public:
    ThrusterMgr(std::unique_ptr<IThruster>, std::unique_ptr<IThruster>,
                std::unique_ptr<IThruster>, std::unique_ptr<IThruster>,
                std::unique_ptr<IThruster>, std::unique_ptr<IThruster>,
                std::unique_ptr<IThruster>, std::unique_ptr<IThruster>);
    ~ThrusterMgr();

    
    virtual bool OperateThruster(const std::vector<uint16_t>);

private:

    using ThrusterList = std::vector<std::unique_ptr<IThruster>>;

    static constexpr uint16_t kThrusterNum = 8;
    enum class ThrusterId;

    ThrusterList thruster_list_;

};

enum class ThrusterId
{
    kThr_1,
    kThr_2,

};

#endif /* THRUSTER_MGR_H_ */