/******************************************************************************
 |
 | 	FILENAME:  synch.cc
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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace adara::rtos;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Constructor(s) / Destructor -----------------------------------------------*/
FreeRTOSMutex::FreeRTOSMutex() {
  mut_ = xSemaphoreCreateBinary();
}
FreeRTOSMutex::~FreeRTOSMutex() {
  vSemaphoreDelete(mut_);
}

FreeRTOSSemaphore::FreeRTOSSemaphore(uint32_t const max,
                                     uint32_t const initial) {
  sem_ = xSemaphoreCreateCounting(max, initial);
}
FreeRTOSSemaphore::~FreeRTOSSemaphore() {
  vSemaphoreDelete(sem_);
}
/* Public methods ------------------------------------------------------------*/
/////////////////////// MUTEX  /////////////////////////////////////////////////
void
FreeRTOSMutex::Reset() {
  while(xSemaphoreTake(mut_, 0) == pdTRUE);
}
void
FreeRTOSMutex::Set() {
  xSemaphoreGive(mut_);
}
Status
FreeRTOSMutex::PendFromTask(uint32_t to) {
  return xSemaphoreTake(mut_, to) == pdTRUE ? Status::kOk :
      Status::kTimeout;
}
Status
FreeRTOSMutex::PendFromIsr() {
  BaseType_t hp = pdFALSE;
  Status s = (xSemaphoreTakeFromISR(mut_, &hp) == pdTRUE ? Status::kOk :
      Status::kTimeout);
  portYIELD_FROM_ISR(hp);
  return s;
}
Status
FreeRTOSMutex::PostFromTask(uint32_t to) {
  (void)to;
  return xSemaphoreGive(mut_) == pdTRUE ? Status::kOk : Status::kRunTimeError;
}
Status
FreeRTOSMutex::PostFromIsr() {
  BaseType_t hp = pdFALSE;
  Status s = (xSemaphoreGiveFromISR(mut_, &hp) == pdTRUE ? Status::kOk :
      Status::kRunTimeError);
  portYIELD_FROM_ISR(hp);
  return s;
}

/////////////////////// SEMAPHORE //////////////////////////////////////////////
void
FreeRTOSSemaphore::Reset() {
  while(xSemaphoreTake(sem_, 0) == pdTRUE);
}
Status
FreeRTOSSemaphore::PendFromTask(uint32_t to) {
  return xSemaphoreTake(sem_, to) == pdTRUE ? Status::kOk :
      Status::kTimeout;
}
Status
FreeRTOSSemaphore::PendFromIsr() {
  BaseType_t hp = pdFALSE;
  Status s = (xSemaphoreTakeFromISR(sem_, &hp) == pdTRUE ? Status::kOk :
      Status::kTimeout);
  portYIELD_FROM_ISR(hp);
  return s;
}
Status
FreeRTOSSemaphore::PostFromTask(uint32_t to) {
  (void)to;
  return xSemaphoreGive(sem_) == pdTRUE ? Status::kOk : Status::kRunTimeError;
}
Status
FreeRTOSSemaphore::PostFromIsr() {
  BaseType_t hp = pdFALSE;
  Status s = (xSemaphoreGiveFromISR(sem_, &hp) == pdTRUE ? Status::kOk :
      Status::kRunTimeError);
  portYIELD_FROM_ISR(hp);
  return s;
}
uint32_t
FreeRTOSSemaphore::Count() {
  return uxSemaphoreGetCount(sem_);
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
