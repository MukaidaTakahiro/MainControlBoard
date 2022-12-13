#ifndef BMS_MGR_H_
#define BMS_MGR_H_

#include <stdint.h>
#include <memory>
#include "FreeRTOS.h"
#include "timers.h"

#include "TaskBase.h"

#include "IBmsMgr.h"
#include "IBms.h"

class BmsMgr: public IBmsMgr, public TaskBase
{
public:
    BmsMgr(std::unique_ptr<IBms>);
    ~BmsMgr() = default;

    void Init();
    virtual BmsParam GetBmsParam();

private:
                                            /* BMSパラメータの更新周期[ms]    */
    static constexpr uint16_t kBmsParamUpdateCycle = 500;

    BmsParam bms_param_;
    std::unique_ptr<IBms> bms_;
    SemaphoreHandle_t bms_param_mutex_;

    TimerHandle_t update_param_timer_;

    /* Private関数 */
    virtual void PerformTask();
    void UpdateBmsParam();
    static void TimeoutEntryPoint(TimerHandle_t);
    void RequestParamUpdate();
};

#endif /* BMS_MGR_H_ */