/******************************************************************************
 |
 |  	FILENAME:  digitalconecan.h
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
#ifndef DIGITALCONE_DIGITALCONECAN_H_
#define DIGITALCONE_DIGITALCONECAN_H_

/* Includes ------------------------------------------------------------------*/
#include <adara/rtos.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
/* Namespace declaration -----------------------------------------------------*/

/* Class definition ----------------------------------------------------------*/
class DigitalConeCan final : public adara::rtos::TaskInterface {
 public:
  DigitalConeCan();
  ~DigitalConeCan() override = default;
  DigitalConeCan(const DigitalConeCan&) = delete;
  DigitalConeCan& operator=(const DigitalConeCan&) = delete;

  virtual adara::rtos::Status Task() override;

 private:
  QueueHandle_t queue_;
};
#endif /* DIGITALCONE_DIGITALCONECAN_H_ */
