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
#include "InternalCommunication.h"
#include "InUartCommunication.h"
#include "HeartBeat.h"
#include "ExternalBoard.h"

class MainRoutine: public TaskBase
{
public:
    MainRoutine();
    ~MainRoutine();

    void Init();
    void StartRoutine();
    void LedBeatRtn();
    void PrbCommTest();

    std::shared_ptr<UartInterrupt> uart_interrupt_;
    std::shared_ptr<ExUartCommunication> ex_uart_comm_;
    std::shared_ptr<ExternalCommunication> ex_comm_;
    std::shared_ptr<ExternalBoard> ex_board_;
    std::shared_ptr<CommandMgr> cmd_mgr_;
    std::shared_ptr<HeartBeat> heart_beat_;


    std::shared_ptr<InUartCommunication> bob_uart_comm_;
    std::shared_ptr<InUartCommunication> prb_uart_comm_;
    std::shared_ptr<InUartCommunication> eob_uart_comm_;
    std::shared_ptr<InternalCommunication> bob_comm_;
    std::shared_ptr<InternalCommunication> prb_comm_;
    std::shared_ptr<InternalCommunication> eob_comm_;
    

private:
    virtual void PerformTask();

};
#endif /* MAIN_ROUTINE_H_ */
