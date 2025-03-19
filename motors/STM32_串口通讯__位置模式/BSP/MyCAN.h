#ifndef __MYCAN_H
#define __MYCAN_H

#include "stm32f10x_can.h"

extern CanRxMsg MyCAN_RxMsg;
extern uint8_t MyCAN_RxFlag;

void MyCAN_Init(void);
void MyCAN_Transmit(CanTxMsg *TxMessage);

#endif
