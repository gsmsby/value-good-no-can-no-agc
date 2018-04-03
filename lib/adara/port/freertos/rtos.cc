/******************************************************************************
 |
 | 	FILENAME:  rtos.cc
 |
 |	Copyright 2017 Adara Systems Ltd. as an unpublished work.
 |	All Rights Reserved.
 |
 |	The information contained herein is confidential property of Adara Systems
 |	Ltd. The use, copying, transfer or disclosure of such information is
 |	prohibited except by express written agreement with Adara Systems Ltd.
 |
 |  	DESCRIPTION:
 |
 |  	PUBLIC FUNCTIONS:
 |
 |
 |  	NOTES:
 |
 |  	AUTHOR(S):  Roque
 |	    DATE:		Dec 7, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "rtos.h"

#include <algorithm>

#include <FreeRTOS.h>
#include <task.h>
/* Private typedef -----------------------------------------------------------*/
using namespace adara::rtos;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern "C" {
  void FreeRTOS_TaskEntry(void *);
}

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
Status
FreeRTOS::TaskStart(TaskInterface & task, const std::string & name,
                    uint32_t const stacksize, TaskPriority const priority) {
  TaskNode t = {
      .task = task,
      .thandle = nullptr,
  };
  if (SchedulerRunning())
    mut_.PendFromTask();

  tasklist_.push_back(std::move(t));
  xTaskCreate(FreeRTOS_TaskEntry, name.c_str(),
              (uint16_t)(stacksize / sizeof(portSTACK_TYPE)), &tasklist_.back(),
              (UBaseType_t)priority, &tasklist_.back().thandle);

  if (SchedulerRunning())
    mut_.PostFromTask();
  return Status::kOk;
}
Status
FreeRTOS::TaskStop(TaskInterface & task) {
  Status s = Status::kRunTimeError;
  auto tniterator = std::find_if(tasklist_.begin(),
                              tasklist_.end(),
                              [&](TaskNode tlist)->bool
                              {return &task == &tlist.task;});
  if (tniterator != tasklist_.end()) {
    vTaskDelete(tniterator->thandle);
    tasklist_.erase(tniterator);
    s = Status::kOk;
  }
  return s;
}
uint32_t
FreeRTOS::TaskCount() {
  return uxTaskGetNumberOfTasks();
}
void FreeRTOS::TaskSleepFor(std::chrono::milliseconds ms) {
  vTaskDelay((TickType_t)ms.count());
}
void FreeRTOS::TaskSleepUntil(std::chrono::milliseconds ms) {

}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void FreeRTOS_TaskEntry(void * t) {
  FreeRTOS::TaskNode & tasknode = *((FreeRTOS::TaskNode *)t);
  tasknode.task.Task();
  OS::TaskStop(tasknode.task);
}
