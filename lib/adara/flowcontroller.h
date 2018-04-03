/******************************************************************************
 |
 |  	FILENAME:  flowcontroller.h
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
#ifndef ADARA_FLOWCONTROLLER_H_
#define ADARA_FLOWCONTROLLER_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>
#include <forward_list>
/* Namespace declaration -----------------------------------------------------*/
namespace adara {
namespace data {
/* Class definition ----------------------------------------------------------*/
class FlowController;

class ConsumerBase {
 public:
  uint32_t const MaxPacketSize;

  ConsumerBase(uint32_t const psize):MaxPacketSize(psize){}
  virtual ~ConsumerBase() = default;
  virtual int Transmit(uint8_t const * const b, uint32_t const size) = 0;

 protected:
  virtual bool IsReady() = 0;
  virtual int BlockTillReady(uint32_t to = ~0) = 0;
//  virtual void Purge() = 0;

  friend class FlowController;
};

class FlowController final {
 public:
  FlowController(ConsumerBase & consumer);
  ~FlowController() = default;

  void Push(uint8_t const * const b, uint32_t const size);
  void Purge() { extrasize_ = 0; }

 private:
  ConsumerBase & consumer_;
  uint8_t * extrastorage_;
  uint32_t extrasize_;

  void WaitIfRequired();
};
} // namespace data
} // namespace adara
#endif /* ADARA_FLOWCONTROLLER_H_ */
