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

class MainRoutine: public TaskBase
{
public:
    MainRoutine();
    ~MainRoutine();

    void Init();
    void StartRoutine();

    std::shared_ptr<UartInterrupt> uart_interrupt_;
    std::shared_ptr<ExUartCommunication> ex_uart_comm_;
    std::shared_ptr<ExternalCommunication> ex_comm_;
    std::shared_ptr<CommandMgr> cmd_mgr_;
    std::shared_ptr<HeartBeat> heart_beat_;


    std::shared_ptr<InUartCommunication> in_uart_comm_;
    std::shared_ptr<InternalCommunication> internal_comm_;
    

private:
    virtual void PerformTask();

};
#endif /* MAIN_ROUTINE_H_ */
