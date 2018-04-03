/******************************************************************************
 |
 |  	FILENAME:  canslave.h
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
 |  	AUTHOR(S):  William Seo
 |	    DATE:		Feb 28, 2018
 |
 ******************************************************************************/
#ifndef DIGITALCONE_CANBUS_CANSLAVE_H_
#define DIGITALCONE_CANBUS_CANSLAVE_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <vector>
#include <deque>

/* Namespace declaration -----------------------------------------------------*/
namespace digitalcone {
/* Class definition ----------------------------------------------------------*/
class CanSlaveHardware {
 public:
  virtual ~CanSlaveHardware(){}

  virtual void Init() = 0;
  virtual void WriteMessage(uint32_t const id, uint8_t const * const dat,
                            uint32_t const size) = 0;
  virtual float DataRequest(uint8_t const channel) = 0;
  virtual void GetChipID(uint64_t * const chipID,
                         uint32_t const length) = 0;
  virtual void GetSpecSheet(uint8_t const * * const buf,
                            uint32_t * const size) = 0;
};

class CanSlave {
 public:
  CanSlave(CanSlaveHardware & p);
  ~CanSlave();

  // no copy constructor, no assignment
  CanSlave(CanSlave const &) = delete;
  CanSlave& operator=(CanSlave const &) = delete;

  // Public API
  void Connect() { RequestTable(); }
  void RxData(uint32_t const id, uint8_t const * const dat,
  						uint32_t const size);

 private:
  void RequestTable();
  void ReceiveSlaveTable(uint8_t const * const dat, uint8_t const size);
  void ReceiveExtraSlaveTable(uint8_t const * const dat, uint32_t const size);
  uint32_t ReceiveAddressConfirm(uint8_t const * const dat, uint8_t const size);
  void TableTrim();
  void HandShaking();
  void SetCanID(uint32_t newid);
  uint32_t GetCanID() const;
  void SendSelectedAddress(uint8_t const wishid);
  void SendSpecSheet();
  void SendChannelData(uint32_t const id, uint8_t const channel);

 private:
  uint8_t canid_;
  bool extratable_;
  uint8_t tablecapacity_;
  std::deque<uint8_t> idtable_;
  CanSlaveHardware & appcallbacks_;
};
} //namespace digitalcone
#endif /* DIGITALCONE_CANBUS_CANSLAVE_H_ */
