/*
 * TaskBase.cpp
 *
 *  Created on: Sep 27, 2022
 *      Author: 向田隆浩
 */

#include "TaskBase.h"

TaskBase::TaskBase(const char *name, int priority, 
			uint32_t stack_size = configMINIMAL_STACK_SIZE)
: name_(name), priority_(priority), stack_size_(stack_size)
{
}

TaskBase::~TaskBase()
{
	DeleteTask();
}

void TaskBase::CreateTask()
{
	xTaskCreate(TaskEntryPoint, name_, stack_size_, this, priority_, &handle_);
}

void TaskBase::DeleteTask()
{
	vTaskDelete(handle_);
}

void TaskBase::TaskEntryPoint(void* task_instance)
{
	reinterpret_cast<TaskBase*>(task_instance)->PerformTask();
}