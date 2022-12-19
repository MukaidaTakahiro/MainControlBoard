#ifndef MAIN_ROUTINE_H_
#define MAIN_ROUTINE_H_

#include <memory>
#include <stdint.h>

#include "DebugFunc.h"

#include "TaskBase.h"
#include "UartInterrupt.h"
#include "ExUartCommunication.h"
#include "ExternalCommunication.h"
#include "CommandMgr.h"
#include "BmsMgr.h"
#include "Bms.h"
#include "Thruster.h"
#include "ThrusterMgr.h"
#include "InternalCommunication.h"
#include "InUartCommunication.h"
#include "HeartBeat.h"
#include "ExternalBoard.h"
#include "ResetIc.h"

class MainRoutine: public TaskBase
{
public:
    MainRoutine();
    ~MainRoutine();

    void Init();
    void StartRoutine();
    void LedBeatRtn();
    void PrbCommTest();

    UartInterrupt& uart_interrupt_;


    std::unique_ptr<ExUartCommunication> ex_uart_comm_;
    std::unique_ptr<ExternalCommunication> ex_comm_;

    std::unique_ptr<InUartCommunication> bob_uart_comm_;
    std::unique_ptr<InUartCommunication> prb_uart_comm_;
    std::unique_ptr<InUartCommunication> eob_uart_comm_;
    std::unique_ptr<InternalCommunication> bob_comm_;
    std::unique_ptr<InternalCommunication> prb_comm_;
    std::unique_ptr<InternalCommunication> eob_comm_;    
    


    std::unique_ptr<ExternalBoard> ex_board_;
    std::unique_ptr<Bms> bms_;
    std::unique_ptr<BmsMgr> bms_mgr_;
    std::unique_ptr<ResetIc> reset_ic_;
    std::unique_ptr<HeartBeat> heart_beat_;

    std::unique_ptr<Thruster> thruster_0_;
    std::unique_ptr<Thruster> thruster_1_;
    std::unique_ptr<Thruster> thruster_2_;
    std::unique_ptr<Thruster> thruster_3_;
    std::unique_ptr<Thruster> thruster_4_;
    std::unique_ptr<Thruster> thruster_5_;
    std::unique_ptr<Thruster> thruster_6_;
    std::unique_ptr<Thruster> thruster_7_;

    std::unique_ptr<ThrusterMgr> thruster_mgr_;
    
    std::unique_ptr<CommandMgr> cmd_mgr_;

private:
    virtual void PerformTask();

};
#endif /* MAIN_ROUTINE_H_ */
