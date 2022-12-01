/*
 * TaskBase.h
 *
 *  Created on: Sep 27, 2022
 *      Author: 向田隆浩
 */

#ifndef TASKBASE_H_
#define TASKBASE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

class TaskBase {
public:
	// デフォルトコンストラクタを禁止
	// 継承先のコンストラクタでTaskBaseの初期化を強制するため
	TaskBase() = delete;

	TaskBase(const char *, int, uint32_t);
	virtual ~TaskBase();

protected:
	xTaskHandle handle_ = 0;
	const char* name_;
	int priority_;
	uint32_t stack_size_;

	virtual void PerformTask() = 0;

	void CreateTask();
	void DeleteTask();

	static void TaskEntryPoint(void*);
};

#endif /* TASKBASE_H_ */
