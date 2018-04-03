/******************************************************************************
 |
 | 	FILENAME:  canslave.cc
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
/* Includes ------------------------------------------------------------------*/
#include "canslave.h"

#include <algorithm>

/* Private typedef -----------------------------------------------------------*/
using namespace digitalcone;
/* Private define ------------------------------------------------------------*/


#define CANID_MASTERBROADCAST   0
#define CANID_UNINITSLAVE       1

#define CMD_REQUESTTABLE        1
#define CMD_SENDTABLE           2
#define CMD_SELECTADDRESS       3
#define CMD_CONFIMADDRESSSEL    4
#define CMD_SENDSPECSHEET       5
#define CMD_REQSPECSHEET        6
#define CMD_SENDCHANNELDATA     7
#define CMD_REQUESTCHANNELDATA  8

#define ACK                     1
#define NACK                    0

#define TXMAXSIZE               100
/* Private macro ----------------s---------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
CanSlave::CanSlave(CanSlaveHardware & p)
:canid_(CANID_MASTERBROADCAST), extratable_(false), tablecapacity_(0),
 appcallbacks_(p) {
  appcallbacks_.Init();
}
CanSlave::~CanSlave() {
}
/* Public methods ------------------------------------------------------------*/
void CanSlave::RxData(uint32_t const id, uint8_t const * const dat,
                      uint32_t const size) {
  if(id == GetCanID()) {
    if(extratable_ == true) {
      ReceiveExtraSlaveTable(&dat[0], size);
    } else {
      switch(dat[0]) {
        case CMD_SENDTABLE:
          ReceiveSlaveTable(&dat[2], dat[1]);
          break;

        case CMD_CONFIMADDRESSSEL:
          ReceiveAddressConfirm(&dat[1], size - 1);
          break;

        case CMD_REQSPECSHEET:
          SendSpecSheet();
          break;

        case CMD_REQUESTCHANNELDATA:
          uint8_t const ch = dat[1];
          SendChannelData(GetCanID(), ch);
          break;
      }
    }
  } else {
    // the message is not for me so do nothing
    return;
  }
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void CanSlave::RequestTable() {
	// header
	uint8_t cmd = CMD_REQUESTTABLE;

	// message init
	uint8_t messageframesize = sizeof(cmd);
	uint8_t messagebuff[messageframesize];

	// message header write
	messagebuff[0] = cmd;

	// send message
	appcallbacks_.WriteMessage(CANID_UNINITSLAVE, messagebuff, messageframesize);
}
void CanSlave::ReceiveSlaveTable(uint8_t const * const dat, uint8_t const size)
{
	idtable_.clear();
	tablecapacity_ = size;
	if(size <= 6) {
	  std::copy(dat, dat + size, std::back_inserter(idtable_));
    tablecapacity_ -= size;
    TableTrim();
		extratable_ = false;

		HandShaking();
	} else {
	  std::copy(dat, dat + 6, std::back_inserter(idtable_));
		tablecapacity_ -= 6;
		extratable_ = true;
	}
}
void CanSlave::ReceiveExtraSlaveTable(uint8_t const * const dat,
																			uint32_t const size) {
	for(uint8_t iter = 0; iter < size; ++iter) {
		idtable_.push_back(dat[iter]);
		--tablecapacity_;
	}

	if(tablecapacity_ > 0) {
		extratable_ = true;
	} else {
		extratable_ = false;
		TableTrim();
	}
}
uint32_t CanSlave::ReceiveAddressConfirm(uint8_t const * const dat,
                                         uint8_t const size) {
	uint32_t const chipidsize = 6;
	uint64_t chipid;
	uint8_t const * const pcid = (uint8_t *)&chipid;
	uint8_t ack;
	appcallbacks_.GetChipID(&chipid, chipidsize);
	for(uint8_t iter = 0; iter < chipidsize; ++iter) {
		if(pcid[iter] != dat[iter])
			return 1;
	}
	ack = dat[size - 1];
	if (ack == 0xff) {
		HandShaking();
	} else if (ack == 0) {
	  SetCanID(45);
		return 0;
	} else {
		return 2;
	}
}
void CanSlave::TableTrim() {
	std::sort(idtable_.begin(), idtable_.end());
	// get rid of canid 0 and 1
	idtable_.pop_front();
	idtable_.pop_front();
}
void CanSlave::HandShaking() {
		uint8_t wishid = 45;
//		SetCanID(wishid);
		SendSelectedAddress(wishid);
}
void CanSlave::SetCanID(uint32_t newid) {
	canid_ = newid;
	return;
}
uint32_t CanSlave::GetCanID() const{
	return canid_;
}
void CanSlave::SendSelectedAddress(uint8_t const wishid) {
	// header
	uint8_t cmd = CMD_SELECTADDRESS;
	uint32_t const chipidsize = 6;
	uint64_t chipid;
	uint8_t const * const pch = (uint8_t *)&chipid;
	appcallbacks_.GetChipID(&chipid, chipidsize);
	// NOTE: even though actual chipid length is more than 6 byte
	// our chipid length is limited to 6 bytes

	// message init
	uint32_t messageframesize = sizeof(cmd) + sizeof(wishid) + chipidsize;
	uint8_t messagebuff[messageframesize];

	// message header write
	messagebuff[0] = cmd;
	messagebuff[1] = wishid;

	// message body write
	for(uint8_t iter = 0; iter < chipidsize; ++iter) {
		messagebuff[iter + 2] = pch[iter];
	}
	// send message
	appcallbacks_.WriteMessage(CANID_UNINITSLAVE, messagebuff, messageframesize);
}
void CanSlave::SendSpecSheet() {
	// canid
	uint32_t canid = GetCanID();

	// header
	uint8_t cmd = CMD_SENDSPECSHEET;

	// body
  uint32_t pspecsheetlength;
  uint8_t const * pspecsheetdata;
	appcallbacks_.GetSpecSheet(&pspecsheetdata, &pspecsheetlength);

	// message init
	constexpr uint32_t headerlength = sizeof(uint8_t) + sizeof(uint16_t);
	uint32_t messageframesize = pspecsheetlength + headerlength;
	uint8_t *messagebuff = new uint8_t[messageframesize];

	// message header write
	messagebuff[0] = cmd;
	std::copy((uint8_t *)&pspecsheetlength,
	          (uint8_t *)&pspecsheetlength + sizeof(uint16_t),
	          &messagebuff[1]);

	// message body write
	std::copy(pspecsheetdata, pspecsheetdata + pspecsheetlength,
	          &messagebuff[3]);
//	for(uint16_t iter = 0; iter < pspecsheetlength; ++iter) {
//		messagebuff[iter + 3] = *(pspecsheetdata + iter);
//	}

	// send message
	appcallbacks_.WriteMessage(canid, messagebuff, messageframesize);
	delete[] messagebuff;
}
void CanSlave::SendChannelData(uint32_t const id, uint8_t const channel)
{
  // canid
	uint32_t canid = GetCanID();

	// header
	uint8_t cmd = CMD_SENDCHANNELDATA;

	// body
	float data = appcallbacks_.DataRequest(channel);

	// message init
	constexpr uint8_t messageframesize = sizeof(cmd) + sizeof(channel) +
	                                     sizeof(data);
	uint8_t messagebuff[messageframesize];
	// message header write
	messagebuff[0] = CMD_SENDCHANNELDATA;
	messagebuff[1] = channel;
	// message body write
	std::copy((uint8_t *)&data, (uint8_t *)&data + sizeof(data),
	          &messagebuff[2]);
//	uint8_t *pf = (uint8_t *)&data;
//	for(int iter = 0; iter < (uint8_t)sizeof(float); ++iter)
//		messagebuff[iter + 2] = *(pf + iter);
	// send message
  appcallbacks_.WriteMessage(canid, messagebuff, messageframesize);
}
