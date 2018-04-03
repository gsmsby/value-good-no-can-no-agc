
/******************************************************************************
 |
 |    FILENAME:  taskutil.h
 |
 |  Copyright 2017 Adara Systems Ltd. as an unpublished work.
 |  All Rights Reserved.
 |
 |  The information contained herein is confidential property of Adara Systems
 |  Ltd. The use, copying, transfer or disclosure of such information is
 |  prohibited except by express written agreement with Adara Systems Ltd.
 |
 |    DESCRIPTION:
 |
 |    PUBLIC FUNCTIONS:
 |
 |
 |    NOTES:
 |
 |    AUTHOR(S):  Roque
 |      DATE:   Dec 11, 2017
 |
 ******************************************************************************/
#ifndef INCLUDE_ADARA_TASKUTIL_H_
#define INCLUDE_ADARA_TASKUTIL_H_
/* Includes ------------------------------------------------------------------*/
#include "rtos.h"
/* Namespace declaration -----------------------------------------------------*/
namespace adara {
namespace osutils {
/* Class definition ----------------------------------------------------------*/
template <typename LambdaTask>
class ASyncTask : public adara::rtos::TaskInterface {
public:
  ASyncTask(LambdaTask t):taskwork_(t), taskrunning_(false) {
    rtos::OS::TaskStart(*this, "ASyncRunner", 512, rtos::TaskPriority::kHigh);
  }
  ~ASyncTask() {
    rtos::OS::TaskStop(*this);
  }

  void EndTask() { rtos::OS::TaskStop(*this); }

  virtual adara::rtos::Status Task() override {
    taskrunning_ = true;
    taskwork_();
    taskrunning_ = false;
    return rtos::Status::kOk;
  }
private:
  LambdaTask taskwork_;
  bool taskrunning_;
};

} // namespace osutils
} // namespace adara
#endif /* INCLUDE_ADARA_TASKUTIL_H_ */
