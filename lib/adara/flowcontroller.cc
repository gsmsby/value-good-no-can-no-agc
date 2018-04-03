/******************************************************************************
 |
 | 	FILENAME:  flowcontroller.cc
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
 |	    DATE:		Dec 8, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "flowcontroller.h"
#include <segger/SEGGER_RTT.h>
/* Private typedef -----------------------------------------------------------*/
using namespace adara::data;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
FlowController::FlowController(ConsumerBase & consumer)
:consumer_(consumer), extrasize_(0){
  extrastorage_ = new uint8_t[consumer.MaxPacketSize];
}

/* Public methods ------------------------------------------------------------*/
void FlowController::Push(uint8_t const * const b, uint32_t const size) {
  uint32_t const maxpacket = consumer_.MaxPacketSize;
  uint8_t const * pb = b;
  uint32_t s = size;

  if (extrasize_ > 0) {
    auto srequired = maxpacket - extrasize_;
    srequired = std::min(s, srequired);
    extrasize_ += srequired;
    s -= srequired;
    std::copy(pb, pb + srequired, &extrastorage_[extrasize_]);
    if (extrasize_ == maxpacket) {
      consumer_.Transmit(extrastorage_, maxpacket);
      WaitIfRequired();
    }
  }

  uint32_t const iter = s / maxpacket;
  uint32_t const extra = s % maxpacket;
  for (uint32_t i = 0 ; i < iter ; ++i, pb += maxpacket) {
    consumer_.Transmit(pb, maxpacket);
    WaitIfRequired();
  }

  if (extra > 0) {
    extrasize_ = extra;
    std::copy(pb, pb + extra, extrastorage_);
  }
}


/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void FlowController::WaitIfRequired() {
  if (consumer_.IsReady() == false) {
    consumer_.BlockTillReady();
  }
}
