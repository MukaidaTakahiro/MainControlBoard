#include "MainRoutine.h"


MainRoutine::MainRoutine()
:	TaskBase("MainRoutine", 1, 128)
{
}

MainRoutine::~MainRoutine()
{
}

void MainRoutine::Init()
{
    uart_interrupt_ = UartInterrupt::GetInstance();
    
    ex_uart_comm_ = std::make_shared<ExUartCommunication>(uart_interrupt_, &huart3);
    ex_uart_comm_->Init();
    
    ex_comm_ = std::make_shared<ExternalCommunication>(ex_uart_comm_);
    ex_comm_->Init();

    cmd_mgr_ = std::make_shared<CommandMgr>(ex_comm_, 
                                            nullptr, 
                                            nullptr, 
                                            nullptr, 
                                            nullptr);

    cmd_mgr_->Init();
    
    CreateTask();
}

void MainRoutine::StartRoutine()
{
    vTaskStartScheduler();
}

void MainRoutine::PerformTask()
{
    std::vector<uint8_t> msg;
    while (1)
    {
        DebugLedRed(100);
        /*
        while (!ex_uart_comm_->IsUartEmpty())
        {
            msg.push_back(ex_uart_comm_->ReadByte());
        }
        
        ex_uart_comm_->SendMsg(msg);
        msg.clear();

        ex_uart_comm_->WaitReceiveData(portMAX_DELAY);
        */
    }
}
