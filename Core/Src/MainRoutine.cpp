#include "MainRoutine.h"

void DebugLedRed(uint16_t);
void DebugLedBlue(uint16_t);
void DebugLedRB(uint16_t);

MainRoutine::MainRoutine()
:	TaskBase("MainRoutine", 1, 256)
{
}

MainRoutine::~MainRoutine()
{
}

void MainRoutine::Init()
{
	
	uart_interrupt_ = UartInterrupt::GetInstance();
    
    ex_uart_comm_ = std::make_shared<ExUartCommunication>(uart_interrupt_, &huart1);
    ex_uart_comm_->Init();

    prb_uart_comm_ = std::make_shared<InUartCommunication>(uart_interrupt_, &huart3);
    prb_uart_comm_->Init();


    ex_comm_ = std::make_shared<ExternalCommunication>(ex_uart_comm_);
    ex_comm_->Init();

    in_comm_ = std::make_shared<InternalCommunication>(prb_uart_comm_);

    ex_board_ = std::make_shared<ExternalBoard>(nullptr, in_comm_, nullptr);

    cmd_mgr_ = std::make_shared<CommandMgr>(ex_comm_, 
                                            nullptr, 
                                            ex_board_, 
                                            nullptr, 
                                            nullptr);

    cmd_mgr_->Init();

    uart_interrupt_->Init();

    CreateTask();
}

void MainRoutine::StartRoutine()
{
    vTaskStartScheduler();
}

void MainRoutine::PerformTask()
{
    LedBeatRtn();
	//PrbCommTest();
}

void MainRoutine::LedBeatRtn()
{
    while (1)
    {
        DebugLedBlue(500);
    }
    
}

void MainRoutine::PrbCommTest()
{
    uint16_t state = 0;
    while (1)
    {
        std::vector<uint8_t> ex_msg;
        std::vector<uint8_t> in_msg;

        DebugLedBlue(100);

        switch (state)
        {
        case 0:

            while (1)
            {
                while (!ex_uart_comm_->IsUartEmpty())
                {
                    ex_msg.push_back(ex_uart_comm_->ReadByte());
                }
                if (ex_msg.size() >= 7) break;
                
                ex_uart_comm_->WaitReceiveData(portMAX_DELAY);
            }

            prb_uart_comm_->SendMsg(ex_msg);
            state = 1;

            break;
        
        case 1:

            if (prb_uart_comm_->WaitReceiveData(pdMS_TO_TICKS(1000)))
            {
                while (!prb_uart_comm_->IsUartEmpty())
                {
                    in_msg.push_back(prb_uart_comm_->ReadByte());
                }
            }
            else
            {
                in_msg.push_back(0xCC);
            }
            
            ex_uart_comm_->SendMsg(in_msg);

            state = 0;

            break;
            

        default:
            break;
        }
    }
}

void DebugLedRed(uint16_t time)
{
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
    HAL_Delay(time);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
    HAL_Delay(time);
}

void DebugLedBlue(uint16_t time)
{
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
    HAL_Delay(time);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
    HAL_Delay(time);
}

void DebugLedRB(uint16_t time)
{
    HAL_GPIO_WritePin(GPIOA, LED3_Pin | LED4_Pin, GPIO_PIN_SET);
    HAL_Delay(time);
    HAL_GPIO_WritePin(GPIOA, LED3_Pin | LED4_Pin, GPIO_PIN_RESET);
    HAL_Delay(time);
}
