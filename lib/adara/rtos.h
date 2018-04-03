/******************************************************************************
 |
 |  	FILENAME:  adaralib.h
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
#ifndef LIB_RTOS_H_
#define LIB_RTOS_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
/* Namespace declaration -----------------------------------------------------*/
namespace adara {
namespace rtos {
/* Class definition ----------------------------------------------------------*/
class TaskInterface;
class RTOS;
class SynchFactory;
class Mutex;
class Semaphore;

enum class Status {
  kOk,
  kRunTimeError,
  kTimeout,
  kNotSupported,
};

enum class TaskPriority {
  kIdle,
  kLowest,
  kLow,
  kLowMedium,
  kMedium,
  kHigh,
  kHighest,
};

//struct TaskAutoStartParams {
//  TaskInterface & task;
//  const std::string name;
//  uint32_t const stacksize;
//  TaskPriority const priority;
//};
//typedef std::vector<TaskAutoStartParams> TaskList;

class OS {
 public:
  static Status TaskStart(TaskInterface & task, const char * taskname,
                          uint32_t const stacksize, TaskPriority const prio);
  static Status TaskStop(TaskInterface & task);
  static uint32_t TaskCount();
  static Mutex & MutexCreate();
  static void MutexDelete(Mutex & mut);
  static Semaphore & SemaphoreCreate();
  static void SemaphoreDelete(Semaphore & sem);
  static void TaskSleepFor(std::chrono::milliseconds ms);
  static void TaskSleepUntil(std::chrono::milliseconds ms);

 private:
  static RTOS * rtos_;
  static SynchFactory * synchfactory_;

  friend class RTOS;
  friend class SynchFactory;
};

class Mutex {
 public:
  virtual ~Mutex() = default;

  virtual void Reset() = 0;
  virtual void Set() = 0;

  virtual Status PendFromTask(uint32_t to = ~0) = 0;
  virtual Status PendFromIsr() = 0;
  virtual Status PostFromTask(uint32_t to = 0) = 0;
  virtual Status PostFromIsr() = 0;
};

class Semaphore {
 public:
  virtual ~Semaphore() = default;

  virtual void Reset() = 0;

  virtual Status PendFromTask(uint32_t to = ~0) = 0;
  virtual Status PendFromIsr() = 0;
  virtual Status PostFromTask(uint32_t to = 0) = 0;
  virtual Status PostFromIsr() = 0;

  virtual uint32_t Count() = 0;
};

class SynchFactory {
 public:
  SynchFactory() { OS::synchfactory_ = this; }
  virtual ~SynchFactory() = default;
  virtual Mutex * MutexCreate() = 0;
  virtual void MutexDelete(Mutex * m) = 0;
  virtual Semaphore * SemaphoreCreate() = 0;
  virtual void SemaphoreDelete(Semaphore * s) = 0;
};

class TaskInterface {
 public:
  virtual ~TaskInterface() = default;

 public:
  virtual Status Task() = 0;
};

class RTOS {
 public:
  RTOS() { OS::rtos_ = this; }
  virtual ~RTOS() = default;

  virtual Status TaskStart(TaskInterface & task, const std::string & name,
                           uint32_t const stacksize,
                           TaskPriority const priority) = 0;
  virtual Status TaskStop(TaskInterface & task) = 0;
  virtual uint32_t TaskCount() = 0;

  virtual void TaskSleepFor(std::chrono::milliseconds ms) = 0;
  virtual void TaskSleepUntil(std::chrono::milliseconds ms) = 0;
};

//template<class T>
//void RTOSAutoStart(T tasklist) {
//  TaskList & tl = tasklist.TaskAutoStart();
//  for (auto p : tl) {
//    pv::rtos_->TaskStart(p.task, p.name, p.stacksize, p.priority);
//  }
//}

} // namespace rtos
} // namespace adara
#endif /* LIB_RTOS_H_ */
