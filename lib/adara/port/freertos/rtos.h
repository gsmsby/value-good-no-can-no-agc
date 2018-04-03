/******************************************************************************
 |
 |  	FILENAME:  rtos.h
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
 |	    DATE:		Dec 6, 2017
 |
 ******************************************************************************/
#ifndef ADARA_PORT_FREERTOS_RTOS_H_
#define ADARA_PORT_FREERTOS_RTOS_H_

/* Includes ------------------------------------------------------------------*/
#include <adara/rtos.h>

#include <list>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
/* Namespace declaration -----------------------------------------------------*/

/* Class definition ----------------------------------------------------------*/
class FreeRTOSMutex final : public adara::rtos::Mutex {
 public:
  FreeRTOSMutex();
  virtual ~FreeRTOSMutex() override;

  virtual void Reset() override;
  virtual void Set() override;

  virtual adara::rtos::Status PendFromTask(uint32_t to = ~0) override;
  virtual adara::rtos::Status PendFromIsr() override;
  virtual adara::rtos::Status PostFromTask(uint32_t to = 0) override;
  virtual adara::rtos::Status PostFromIsr() override;

 private:
  SemaphoreHandle_t mut_;
};

class FreeRTOSSemaphore final : public adara::rtos::Semaphore {
 public:
  FreeRTOSSemaphore(uint32_t const max, uint32_t const initial);
  virtual ~FreeRTOSSemaphore() override;

  virtual void Reset() override;

  virtual adara::rtos::Status PendFromTask(uint32_t to = ~0) override;
  virtual adara::rtos::Status PendFromIsr() override;
  virtual adara::rtos::Status PostFromTask(uint32_t to = 0) override;
  virtual adara::rtos::Status PostFromIsr() override;
  virtual uint32_t Count() override;

 private:
  SemaphoreHandle_t sem_;
};

class FreeRTOSSynchFactory final : public adara::rtos::SynchFactory {
 public:
  FreeRTOSSynchFactory() = default;
  virtual ~FreeRTOSSynchFactory() = default;
  virtual adara::rtos::Mutex * MutexCreate() override
      { return new FreeRTOSMutex; }
  virtual adara::rtos::Semaphore * SemaphoreCreate() override
      { return new FreeRTOSSemaphore(10, 0); }
  virtual void MutexDelete(adara::rtos::Mutex * m) override
      { delete m; }
  virtual void SemaphoreDelete(adara::rtos::Semaphore * s) override
      { delete s; }
};

extern "C" void FreeRTOS_TaskEntry(void *);

class FreeRTOS : public adara::rtos::RTOS {
 public:
  template <typename T>
  FreeRTOS(T && lambda) {
    mut_.PostFromTask();
    lambda();
  }
  virtual ~FreeRTOS() = default;

  virtual adara::rtos::Status TaskStart(
      adara::rtos::TaskInterface & task,
      const std::string & name,
      uint32_t const stacksize,
      adara::rtos::TaskPriority const priority) override;
  virtual adara::rtos::Status
  TaskStop(adara::rtos::TaskInterface & task) override;
  virtual uint32_t TaskCount() override;
  virtual void TaskSleepFor(std::chrono::milliseconds ms) override;
  virtual void TaskSleepUntil(std::chrono::milliseconds ms) override;

 private:
  typedef struct {
    adara::rtos::TaskInterface & task;
    TaskHandle_t thandle;
  } TaskNode;
  typedef std::list<TaskNode> FreeRTOSTaskList_t;

  FreeRTOSTaskList_t tasklist_;
  FreeRTOSMutex mut_;
  FreeRTOSSynchFactory synchfact_;

 private:
  bool SchedulerRunning() const {
    return xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
  }
  friend void FreeRTOS_TaskEntry(void *);
};
#endif /* ADARA_PORT_FREERTOS_RTOS_H_ */
