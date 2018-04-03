/******************************************************************************
 |
 | 	FILENAME:  digitalconecan.cc
 |
 |	Copyright 2018 Adara Systems Ltd. as an unpublished work.
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
 |	    DATE:		Mar 7, 2018
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "digitalconecan.h"

#include <stm32f4xx.h>
#include <segger/SEGGER_RTT.h>

#include <resmod/digitalcone/can/candriver.h>
#include <resmod/digitalcone/can/canslavehardware.h>
#include <resmod/digitalcone/can/canslave.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace adara;
using namespace std::chrono_literals;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
DigitalConeCan::DigitalConeCan() {
  queue_ = xQueueCreate(10, sizeof(CanRxMsg));
}
/* Public methods ------------------------------------------------------------*/
rtos::Status
DigitalConeCan::Task() {
  CanDriver candriver;
  BlackBoxCanSlaveHardware canslavehw(candriver);
  digitalcone::CanSlave canslave(canslavehw);

  candriver.ASyncReceieve([this](auto rx) {
    BaseType_t hp = pdFALSE;
    xQueueSendFromISR(queue_, &rx, &hp);
    portYIELD_FROM_ISR(hp);
  });
  rtos::OS::TaskSleepFor(2s);
  canslave.Connect();

  for ( ; ; ) {
    CanRxMsg rx;
    xQueueReceive(queue_, &rx, ~0);
    SEGGER_RTT_printf(0, "\nId: %d, \nSize: %d\n", rx.StdId, rx.DLC);
    canslave.RxData(rx.StdId, rx.Data, rx.DLC);
  }

  return rtos::Status::kOk;
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
