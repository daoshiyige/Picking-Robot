/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "usart.h"

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];


uint8_t receive_flag=0;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 2;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = ENABLE;
  hcan.Init.AutoRetransmission = ENABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
	/*配置CAN过滤器*/
	CAN_FilterTypeDef  sFilterConfig;  
	sFilterConfig.FilterBank = 0;                   //使用过滤器0  
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;         //配置为掩码模式  
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;        //设为32位宽  
	sFilterConfig.FilterIdHigh =((4<<3) >>16) &0xffff;
	sFilterConfig.FilterIdLow =((4<<3)&0xffff) | CAN_ID_EXT; 
	sFilterConfig.FilterMaskIdHigh = (0X7FF>>16)&0xffff;  
	sFilterConfig.FilterMaskIdLow = (0X7FF&0xffff)|CAN_ID_EXT;       //只接收数据帧  
	sFilterConfig.FilterFIFOAssignment = 0;  
	sFilterConfig.FilterActivation = ENABLE;  
	sFilterConfig.SlaveStartFilterBank = 14;  
	if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* 启动CAN外围设备 */
  if (HAL_CAN_Start(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
	/*启动中断*/
	if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
	{
		Error_Handler();
	}
  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
 * @brief       CAN 发送一组数据
 *   @note      发送格式固定为: 标准ID, 数据帧
 * @param       id      : 扩展(32位)
 * @retval      发送状态 0, 成功; 1, 失败;
 */
uint8_t can_send_msg(uint8_t index,uint8_t master_ID,uint8_t can_ID, uint8_t *msg, uint8_t len)
{
  uint32_t TxMailbox = CAN_TX_MAILBOX0;
    
  TxHeader.StdId = can_ID;         /* 标准标识符 */
  TxHeader.ExtId = (index<<24)|(can_ID<<8)|master_ID;         /* 扩展标识符(29位) 标准标识符情况下，该成员无效*/
  TxHeader.IDE = CAN_ID_EXT;   /* 使用扩展标识符 */
  TxHeader.RTR = CAN_RTR_DATA; /* 数据帧 */
  TxHeader.DLC = len;

  if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, msg, &TxMailbox) != HAL_OK) /* 发送消息 */
  {
    return 1;
  }
  
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) != 3); /* 等待发送完成,所有邮箱(有三个邮箱)为空 */
  
  return 0;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1)
{
	if(hcan1->Instance==CAN1)
	{
		HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData); //接收，CAN邮箱为0
		receive_flag=1;
		//can_send_msg(0x00,0xFF,0x04,RxData,8);
	}
	return ;
}

void filter_config(uint8_t id)
{
	hcan.Instance = CAN1;
  hcan.Init.Prescaler = 2;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = ENABLE;
  hcan.Init.AutoRetransmission = ENABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
		/*配置CAN过滤器*/
	CAN_FilterTypeDef  sFilterConfig;  
	sFilterConfig.FilterBank = 0;                   //使用过滤器0  
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;         //配置为掩码模式  
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;        //设为32位宽  
	sFilterConfig.FilterIdHigh =((id<<3) >>16) &0xffff;
	sFilterConfig.FilterIdLow =((id<<3)&0xffff) | CAN_ID_EXT; 
	sFilterConfig.FilterMaskIdHigh = (0x7FF>>16)&0xffff;  
	sFilterConfig.FilterMaskIdLow = (0x7FF&0xffff)|0x02;       //只接收数据帧  
	sFilterConfig.FilterFIFOAssignment = 0;  
	sFilterConfig.FilterActivation = ENABLE;  
	sFilterConfig.SlaveStartFilterBank = 14;  
	if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* 启动CAN外围设备 */
  if (HAL_CAN_Start(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
	/*启动中断*/
	if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
	{
		Error_Handler();
	}
}
/* USER CODE END 1 */
