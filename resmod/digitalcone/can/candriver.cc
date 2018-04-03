/******************************************************************************
 |
 | 	FILENAME:  candriver.cc
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
#include "candriver.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//extern "C" {
//void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan);
//}
/* Private variables ---------------------------------------------------------*/
CAN_InitTypeDef hcan1;
CanDriver * can1instance = nullptr;

extern "C" {
void CAN1_RX0_IRQHandler();
void CAN1_RX1_IRQHandler();
}
void CAN1_RX0_IRQHandler() {
  CanRxMsg rxm;
  CAN_Receive(CAN1, 0, &rxm);
  can1instance->rxcb_(rxm);
}
void CAN1_RX1_IRQHandler() {
  CanRxMsg rxm;
  CAN_Receive(CAN1, 1, &rxm);
  can1instance->rxcb_(rxm);
}
/* Constructor(s) / Destructor -----------------------------------------------*/
CanDriver::CanDriver() {
  InitializeClocks();
  InitializeGPIO();
  InitializeCAN1();
  can1instance = this;

//  NVIC_Set
  NVIC_SetPriority(CAN1_RX0_IRQn, 5);
//  NVIC_S
  NVIC_EnableIRQ(CAN1_RX0_IRQn);
//  __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
}
CanDriver::~CanDriver() {

}
/* Public methods ------------------------------------------------------------*/
//void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {
//  can1instance->rxcb_(*(hcan->pRxMsg));
//  HAL_CAN_Receive_IT(hcan, 0);
//}
void
CanDriver::SetSpeed(Speed sp) {
//  CAN_InitTypeDef& init = hcan1.Init;
//  bool valid = true;
//
//  switch(sp) {
//    case Speed::k100: {
//
//    } break;
//    case Speed::k250: {
//
//    } break;
//    case Speed::k500: {
//      hcan1.CAN_Prescaler = 6;
//      hcan1.CAN_SJW = CAN_SJW_1tq;
//      hcan1.CAN_BS1 = CAN_BS1_11tq;
//      hcan1.CAN_BS2 = CAN_BS2_2tq;
//    } break;
//    case Speed::k1000: {
//
//    } break;
//    default: {
//      valid = false;
//    }
//  }
//  if (valid) {
//    CAN_Init(CAN1, &hcan1);
//  }
}
void
CanDriver::ASyncReceieve(std::function<void(const CanRxMsg &)>&& cb) {
  rxcb_ = std::move(cb);
//  hcan1.pRxMsg = &itrrx_;
//  HAL_CAN_Receive_IT(&hcan1, 0);
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
  CAN_ITConfig(CAN1, CAN_IT_FMP1, ENABLE);
}
void
CanDriver::WaitForTransmission(const CanMessage & pl,
                               uint32_t const timeoutms) {
  const auto size = pl.payload.size();
  auto rem = size;
  auto numiter = (size / 8) + (size % 8 == 0 ? 0 : 1);
  auto dptr = pl.payload.cbegin();
  for ( uint32_t i = 0 ; i < numiter ; ++i ) {
    CanTxMsg tx;
    tx.StdId = pl.id;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = rem > 8 ? 8 : rem;
    std::copy(dptr, dptr + tx.DLC, tx.Data);
//    hcan1.pTxMsg = &tx;

//    HAL_CAN_Transmit(&hcan1, timeoutms == 0 ? ~0 : timeoutms);
    CAN_Transmit(CAN1, &tx);
    while(CAN_TransmitStatus(CAN1, 0) == CAN_TxStatus_Pending);
    dptr += tx.DLC;
    rem -= tx.DLC;
  }
}
void
CanDriver::WaitForTransmission(uint64_t const id,
                               uint8_t const * const buf,
                               uint32_t const size,
                               uint32_t const timeoutms) {
  uint8_t const * pcurrent = buf;
  auto numiter = (size / 8) + (size % 8 == 0 ? 0 : 1);
  for ( uint32_t i = 0 ; i < numiter ; ++i ) {
    uint32_t const rem = size - (pcurrent - buf);
    CanTxMsg tx;
    tx.StdId = (uint32_t)id;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = rem > 8 ? 8 : rem;
    std::copy(pcurrent, pcurrent + tx.DLC, tx.Data);
    CAN_Transmit(CAN1, &tx);
    while(CAN_TransmitStatus(CAN1, 0) == CAN_TxStatus_Pending);
//    hcan1.pTxMsg = &tx;
//    HAL_CAN_Transmit(&hcan1, timeoutms == 0 ? ~0 : timeoutms);
    pcurrent += tx.DLC;
  }
}
CanDriver::CanMessage
CanDriver::WaitForReception(uint32_t const timeoutms) {
  CanRxMsg rxmsg;
  rxmsg.IDE = CAN_Id_Standard;

  uint8_t pending = 0;
  do {
    pending = CAN_MessagePending(CAN1, CAN_FIFO0);
  } while(pending == 0);
  CAN_Receive(CAN1, CAN_FIFO0, &rxmsg);
//  CanRxMsgTypeDef rx;
//  hcan1.pRxMsg = &rx;
//
//  HAL_CAN_Receive(&hcan1, 0, timeoutms == 0 ? ~0 : timeoutms);

  CanMessage m;
  m.id = rxmsg.StdId;
  std::copy(rxmsg.Data, rxmsg.Data + rxmsg.DLC, std::back_inserter(m.payload));
  return m;
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void
CanDriver::InitializeClocks() {
  RCC_AHB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
}
void
CanDriver::InitializeGPIO() {
  GPIO_InitTypeDef gpioa;
  gpioa.GPIO_Mode = GPIO_Mode_AF;
  gpioa.GPIO_OType = GPIO_OType_PP;
  gpioa.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  gpioa.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioa.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOA, &gpioa);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);
//  gpiod.Alternate = GPIO_AF9_CAN1;
//  gpiod.Mode = GPIO_MODE_AF_PP;
//  gpiod.Pin = GPIO_PIN_0 | GPIO_PIN_1;
//  gpiod.Pull = GPIO_NOPULL;
//  gpiod.Speed = GPIO_SPEED_MEDIUM;
//  HAL_GPIO_Init(GPIOD, &gpiod);
}
void
CanDriver::InitializeCAN1() {
  hcan1.CAN_Prescaler = 6;
  hcan1.CAN_Mode = CAN_Mode_Normal;
  hcan1.CAN_SJW = CAN_SJW_1tq;
  hcan1.CAN_BS1 = CAN_BS1_12tq;
  hcan1.CAN_BS2 = CAN_BS2_2tq;
  hcan1.CAN_TTCM = DISABLE;
  hcan1.CAN_ABOM = DISABLE;
  hcan1.CAN_AWUM = DISABLE;
  hcan1.CAN_NART = DISABLE;
  hcan1.CAN_RFLM = DISABLE;
  hcan1.CAN_TXFP = DISABLE;
  CAN_DeInit(CAN1);
  CAN_Init(CAN1, &hcan1);
  CAN_FilterInitTypeDef filt;
  filt.CAN_FilterActivation = ENABLE;
  filt.CAN_FilterFIFOAssignment = 0;
  filt.CAN_FilterIdHigh = 0;
  filt.CAN_FilterIdLow = 0;
  filt.CAN_FilterMaskIdHigh = 0;
  filt.CAN_FilterMaskIdLow = 0;
  filt.CAN_FilterMode = CAN_FilterMode_IdMask;
  filt.CAN_FilterNumber = 13;
  filt.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInit(&filt);
//  CAN_InitTypeDef& init = hcan1.Init;
//  hcan1.Instance = CAN1;
//  init.Prescaler = 6;
//  init.Mode = CAN_MODE_NORMAL;
//  init.SJW = CAN_SJW_1TQ;
//  init.BS1 = CAN_BS1_11TQ;
//  init.BS2 = CAN_BS2_2TQ;
//  init.TTCM = DISABLE;
//  init.ABOM = DISABLE;
//  init.AWUM = DISABLE;
//  init.NART = DISABLE;
//  init.RFLM = DISABLE;
//  init.TXFP = DISABLE;
//  HAL_CAN_Init(&hcan1);
//
//  CAN_FilterConfTypeDef cf;
//  cf.FilterNumber = 0;
//  cf.FilterMode = CAN_FILTERMODE_IDMASK;
//  cf.FilterScale = CAN_FILTERSCALE_32BIT;
//  cf.FilterIdHigh = 0x0000;
//  cf.FilterIdLow = 0x0000;
//  cf.FilterMaskIdHigh = 0x0000;
//  cf.FilterMaskIdLow = 0x0000;
//  cf.FilterFIFOAssignment = 0;
//  cf.FilterActivation = ENABLE;
//  cf.BankNumber = 14;
//  HAL_CAN_ConfigFilter(&hcan1, &cf);
}
