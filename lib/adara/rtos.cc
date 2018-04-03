/*
 * rtospv.cc
 *
 *  Created on: Dec 11, 2017
 *      Author: roqueo
 */
#include <adara/rtos.h>

using namespace adara::rtos;

RTOS * OS::rtos_ = nullptr;
SynchFactory * OS::synchfactory_ = nullptr;

Status OS::TaskStart(TaskInterface & task, const char * taskname,
                     uint32_t const stacksize, TaskPriority const prio) {
  return rtos_->TaskStart(task, taskname, stacksize, prio);
}

Status OS::TaskStop(TaskInterface & task) {
  return rtos_->TaskStop(task);
}

uint32_t OS::TaskCount() {
  return rtos_->TaskCount();
}

Mutex & OS::MutexCreate() {
  return *(synchfactory_->MutexCreate());
}

void OS::MutexDelete(Mutex & mut) {
  synchfactory_->MutexDelete(&mut);
}

Semaphore & OS::SemaphoreCreate() {
  return *(synchfactory_->SemaphoreCreate());
}

void OS::SemaphoreDelete(Semaphore & sem) {
  synchfactory_->SemaphoreDelete(&sem);
}

void OS::TaskSleepFor(std::chrono::milliseconds ms) {
  rtos_->TaskSleepFor(ms);
}

void OS::TaskSleepUntil(std::chrono::milliseconds ms) {
  rtos_->TaskSleepUntil(ms);
}
