
#include "ThrusterMgr.h"

ThrusterMgr::ThrusterMgr(   std::unique_ptr<IThruster> thruster_0, 
                            std::unique_ptr<IThruster> thruster_1,
                            std::unique_ptr<IThruster> thruster_2, 
                            std::unique_ptr<IThruster> thruster_3,
                            std::unique_ptr<IThruster> thruster_4, 
                            std::unique_ptr<IThruster> thruster_5,
                            std::unique_ptr<IThruster> thruster_6, 
                            std::unique_ptr<IThruster> thruster_7)
{
    thruster_list_.reserve(kThrusterNum);

    thruster_list_.push_back(std::move(thruster_0));
    thruster_list_.push_back(std::move(thruster_1));
    thruster_list_.push_back(std::move(thruster_2));
    thruster_list_.push_back(std::move(thruster_3));
    thruster_list_.push_back(std::move(thruster_4));
    thruster_list_.push_back(std::move(thruster_5));
    thruster_list_.push_back(std::move(thruster_6));
    thruster_list_.push_back(std::move(thruster_7));
}

bool ThrusterMgr::OperateThruster(const std::vector<uint16_t> duty_list)
{
    for (uint16_t i = 0; i < thruster_list_.size(); i++)
    {
        thruster_list_[i]->SetDuty(static_cast<uint32_t>(duty_list[i]));
    }

    return true;
    
}
