/******************************************************************************
 |
 |  	FILENAME:  CanDriver.h
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
 |	    DATE:		Mar 6, 2018
 |
 ******************************************************************************/
#ifndef TEST_CAN_CANDRIVER_H_
#define TEST_CAN_CANDRIVER_H_

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>

#include <functional>
#include <cstdint>
#include <array>
#include <vector>
/* Namespace declaration -----------------------------------------------------*/

/* Class definition ----------------------------------------------------------*/
extern "C" void CAN1_RX0_IRQHandler();
extern "C" void CAN1_RX1_IRQHandler();
class CanDriver {
 public:
  typedef std::vector<uint8_t> CANPayload_t;
  struct CanMessage {
    uint32_t id;
    CANPayload_t payload;
  };
  enum class Speed {
    k100,
    k250,
    k500,
    k1000,
  };

  CanDriver();
  ~CanDriver();

  void SetSpeed(Speed sp);
  void ASyncReceieve(std::function<void(const CanRxMsg &)>&& cb);
  void WaitForTransmission(const CanMessage & pl,
                           uint32_t const timeoutms = 0);
  void WaitForTransmission(uint64_t const id, uint8_t const * const buf,
                           uint32_t const size, uint32_t const timeoutms = 0);
  CanMessage WaitForReception(uint32_t const timeoutms = 0);
 private:
  std::function<void(const CanRxMsg &)> rxcb_;
  void InitializeCAN1();
  void InitializeGPIO();
  void InitializeClocks();

  friend void CAN1_RX0_IRQHandler();
  friend void CAN1_RX1_IRQHandler();
};
#endif /* TEST_CAN_CANDRIVER_H_ */
