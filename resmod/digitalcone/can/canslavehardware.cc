/******************************************************************************
 |
 | 	FILENAME:  canslavehardware.cc
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
/* Includes ------------------------------------------------------------------*/
#include "canslavehardware.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern float resist_can;

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
void
BlackBoxCanSlaveHardware::Init() {
}

void
BlackBoxCanSlaveHardware::WriteMessage(const uint32_t id,
                                       const uint8_t * const dat,
                                       const uint32_t size) {
  candriver_.WaitForTransmission(id, dat, size);
}

float
BlackBoxCanSlaveHardware::DataRequest(const uint8_t) {
  static float rval = 0;
  rval = resist_can;
  return rval;
}

void
BlackBoxCanSlaveHardware::GetChipID(uint64_t * const chipID,
                                    const uint32_t) {
  uint64_t id = 4569246;
  *chipID = id;
}

void
BlackBoxCanSlaveHardware::GetSpecSheet(uint8_t const * * const buf,
                                       uint32_t * const size) {
  *buf = ssheet_;
  *size = ssheetsize_;
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
