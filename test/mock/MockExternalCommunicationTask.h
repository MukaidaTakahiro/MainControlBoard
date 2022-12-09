#ifndef MOCK_EXTERNAL_COMMUNICATION_H_
#define MOCK_EXTERNAL_COMMUNICATION_H_

#include <gmock/gmock.h>

#include "IExternalCommunicationTask.h"

class MockExternalCommunicationTask: public IExternalCommunicationTask
{
public:
    MOCK_METHOD0(CreateExCommTask, void(void));
    MOCK_METHOD0(DeleteExCommTask, void(void));
//    MOCK_METHOD2(RegistTask, bool(TaskCallbackFunc, void*));

    bool RegistTask(TaskCallbackFunc callback_task,
                    void* callback_instance)
    {
        callback_instance_ = callback_instance;
        callback_task_ = callback_task;

        return true;
    }
    
    void TestCallbackTask()
    {
        callback_task_(callback_instance_);
    }
    void* callback_instance_;
    TaskCallbackFunc callback_task_;
};

#endif /* MOCK_EXTERNAL_COMMUNICATION_H_ */