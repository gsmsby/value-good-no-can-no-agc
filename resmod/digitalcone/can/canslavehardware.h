/******************************************************************************
 |
 |  	FILENAME:  canslavehardware.h
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
#ifndef TEST_CAN_CANSLAVEHARDWARE_H_
#define TEST_CAN_CANSLAVEHARDWARE_H_

/* Includes ------------------------------------------------------------------*/
#include "canslave.h"

#include <cstdint>

#include "candriver.h"
/* Namespace declaration -----------------------------------------------------*/

/* Class definition ----------------------------------------------------------*/
class BlackBoxCanSlaveHardware : public digitalcone::CanSlaveHardware {
 public:
  BlackBoxCanSlaveHardware(CanDriver & candriver) : candriver_(candriver) { }
  virtual ~BlackBoxCanSlaveHardware() = default;

  virtual void Init() override;
  virtual void WriteMessage(uint32_t const id, uint8_t const * const dat,
                            uint32_t const size) override;
  virtual float DataRequest(uint8_t const channel) override;
  virtual void GetChipID(uint64_t * const chipID,
                         uint32_t const length) override;
  virtual void GetSpecSheet(uint8_t const * * buf,
                            uint32_t * const size) override;
 private:
  CanDriver & candriver_;
  static const uint8_t ssheet_[];
  static const uint32_t ssheetsize_;
};
#endif /* TEST_CAN_CANSLAVEHARDWARE_H_ */
