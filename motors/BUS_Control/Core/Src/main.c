/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SERVO_MOVE_TIME_WRITE 			1
#define SERVO_MOVE_TIME_READ 				2
#define SERVO_MOVE_TIME_WAIT_WRITE 	7
#define SERVO_MOVE_TIME_WAIT_READ 	8 
#define SERVO_MOVE_START 						11
#define SERVO_MOVE_STOP 						12
#define SERVO_ID_WRITE 							13
#define SERVO_ID_READ 							14
#define SERVO_ANGLE_OFFSET_ADJUST 	17
#define SERVO_ANGLE_OFFSET_WRITE 		18
#define SERVO_ANGLE_OFFSET_READ 		19
#define SERVO_ANGLE_LIMIT_WRITE 		20
#define SERVO_ANGLE_LIMIT_READ 			21
#define SERVO_VIN_LIMIT_WRITE 			22
#define SERVO_VIN_LIMIT_READ 				23
#define SERVO_TEMP_MAX_LIMIT_WRITE 	24
#define SERVO_TEMP_MAX_LIMIT_READ 	25
#define SERVO_TEMP_READ 						26
#define SERVO_VIN_READ 							27
#define SERVO_POS_READ 							28
#define SERVO_OR_MOTOR_MODE_WRITE 	29
#define SERVO_OR_MOTOR_MODE_READ 		30
#define SERVO_LOAD_OR_UNLOAD_WRITE 	31
#define SERVO_LOAD_OR_UNLOAD_READ 	32
#define SERVO_LED_CTRL_WRITE 				33
#define SERVO_LED_CTRL_READ 				34
#define SERVO_LED_ERROR_WRITE 			35
#define SERVO_LED_ERROR_READ 				36

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void command_pros(uint8_t index,uint8_t *Data,uint8_t len);
uint8_t  check_sum_pros(uint8_t *Data,uint8_t len);
void write_feedback(uint8_t *Rx_Data,uint8_t *Tx_Data);
void clear(uint8_t *Data);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t Tx_Buffer[10]={0x55,0x55};
uint8_t Rx_Buffer[10];
uint8_t TxData[8]={0};

uint16_t angel_min=0;
uint16_t angel_max=1000;
uint16_t V_min=4500;
uint16_t V_max=12000;
signed short int speed=500;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	uint8_t index;
	uint8_t master_ID;
	uint8_t can_ID;
	uint8_t device_ID=254;
	uint8_t tag=4;
	
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(receive_flag==1)											//CAN���յ���Ϣ
		{
			receive_flag=0;												//���ձ�־����
			index=RxHeader.ExtId>>24;							//����
			master_ID=(RxHeader.ExtId>>8)&0xFF;		//����ID
			can_ID=(RxHeader.ExtId)&0xFF;					//can_ID
			if(can_ID==tag)
			{
				clear(TxData);
				switch(index)
				{
					case 0:
						//HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
						if(RxData[0]==0)
						{														//CAN_ID��ȡ
							TxData[0]=RxData[0];
							TxData[1]=1;
							TxData[7]=can_ID;
						}else{
							Tx_Buffer[2]=device_ID;		//����צ��ID
							Tx_Buffer[3]=3;						//��ȡ��������ݳ��Ⱦ�Ϊ3
							switch(RxData[0])
							{
								case 0x01:
									Tx_Buffer[2]=254;
									Tx_Buffer[4]=SERVO_ID_READ;																				//����צ��ID��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);	//����У���
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,4+3,0xffffffff);								//��������
									device_ID=Rx_Buffer[5];																						//�����豸ID
									command_pros(RxData[0],Rx_Buffer,1);																	//�������ݴ���
									break;
								case 0x02:
									Tx_Buffer[4]=SERVO_ANGLE_OFFSET_READ;															//ƫ��ֵ��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,4+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,1);
									break;
								case 0x03:
									Tx_Buffer[4]=SERVO_ANGLE_LIMIT_READ;															//��С�Ƕ�ֵ��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);	
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,7+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,2);
									break;
								case 0x04:
									Tx_Buffer[4]=SERVO_ANGLE_LIMIT_READ;															//���Ƕ�ֵ��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,7+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,2);
									break;
								case 0x05:
									Tx_Buffer[4]=SERVO_POS_READ;																			//��ǰ�Ƕ�ֵ��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,5+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,2);
									break;
								case 0x06:
									Tx_Buffer[4]=SERVO_VIN_LIMIT_READ;																//��С�����ѹ��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,7+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,2);
									break;
								case 0x07:
									Tx_Buffer[4]=SERVO_VIN_LIMIT_READ;																//��������ѹ��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,7+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,2);
									break;
								case 0x08:
									Tx_Buffer[4]=SERVO_VIN_READ;																			//��ǰ�����ѹ��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,5+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,2);
									break;
								case 0x09:
									Tx_Buffer[4]=SERVO_LED_ERROR_READ;																//����״̬��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,4+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,1);
									break;
								case 0x10:
									Tx_Buffer[4]=SERVO_TEMP_READ;																			//��ǰ�¶ȶ�ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,4+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,1);
									break;
								case 0x11:
									Tx_Buffer[4]=SERVO_OR_MOTOR_MODE_READ;														//��ǰģʽ��ȡ
									Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
									HAL_HalfDuplex_EnableTransmitter(&huart1);
									HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);		//��������
									HAL_HalfDuplex_EnableReceiver(&huart1);
									HAL_UART_Receive(&huart1,Rx_Buffer,7+3,0xffffffff);
									command_pros(RxData[0],Rx_Buffer,1);
									break;
								default:
									break;
							}
						}
						break;
					case 1:
						Tx_Buffer[2]=device_ID;			//����צ��ID
						HAL_HalfDuplex_EnableTransmitter(&huart1);
						switch(RxData[0])
						{
							case 0x00:								//CAN_IDд��
								tag=RxData[7];					
								can_ID=tag;							//CAN_ID����
								HAL_CAN_Stop(&hcan);
								HAL_Delay(100);
								filter_config(tag);			//�������ù�����
								write_feedback(RxData,TxData);//������Ϣ
								break;
							case 0x01:
								Tx_Buffer[3]=4;																											//���ݳ���
								Tx_Buffer[4]=SERVO_ID_WRITE;																				//�޸Ľ���צ��ID
								Tx_Buffer[5]=RxData[7];																							//��ȡ�޸�ID����
								device_ID=RxData[7];																								//����ID
								Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);		//����У���
								HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);			//��������
								write_feedback(RxData,TxData);
								break;
							case 0x02:
								Tx_Buffer[3]=4;																											//ƫ��ֵд��
								Tx_Buffer[4]=SERVO_ANGLE_OFFSET_ADJUST;
								Tx_Buffer[5]=RxData[7];
								Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
								HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);
								Tx_Buffer[3]=3;																											//����ƫ��ֵ
								Tx_Buffer[4]=SERVO_ANGLE_OFFSET_WRITE;
								Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
								HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);			//��������
								write_feedback(RxData,TxData);
								break;
							case 0x03:
								Tx_Buffer[3]=7;																											//��С�Ƕ�д��
								Tx_Buffer[4]=SERVO_ANGLE_LIMIT_WRITE;																
								angel_min=RxData[6]+(RxData[7]<<8);
								if(angel_min>=angel_max)angel_min=angel_max-1;//��֤��С�Ƕ�С�����Ƕ�
								Tx_Buffer[5]=angel_min&0xFF;			//�Ͱ�λ
								Tx_Buffer[6]=(angel_min>>8)&0xFF;	//�߰�λ
								Tx_Buffer[7]=angel_max&0xFF;			//�Ͱ�λ
								Tx_Buffer[8]=(angel_max>>8)&0xFF;	//�߰�λ
								Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
								HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);			//��������
								write_feedback(RxData,TxData);
								break;
							case 0x04:
								Tx_Buffer[3]=7;																											//���Ƕ�д��
								Tx_Buffer[4]=SERVO_ANGLE_LIMIT_WRITE;
								angel_max=RxData[6]+(RxData[7]<<8);
								if(angel_min>=angel_max)angel_max=angel_min+1;//��֤��С�Ƕ�С�����Ƕ�
								Tx_Buffer[5]=angel_min&0xFF;			//�Ͱ�λ
								Tx_Buffer[6]=(angel_min>>8)&0xFF;	//�߰�λ
								Tx_Buffer[7]=angel_max&0xFF;			//�Ͱ�λ
								Tx_Buffer[8]=(angel_max>>8)&0xFF;	//�߰�λ
								Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
								HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);			//��������
								write_feedback(RxData,TxData);
								break;
							case 0x06:
								Tx_Buffer[3]=7;																											//��С��ѹд��
								Tx_Buffer[4]=SERVO_VIN_LIMIT_WRITE;
								V_min=RxData[6]+(RxData[7]<<8);							
								if(V_min>=V_max)V_min=V_max-1;//��֤��С�����ѹС����������ѹ
								Tx_Buffer[5]=V_min&0xFF;			//�Ͱ�λ
								Tx_Buffer[6]=(V_min>>8)&0xFF;	//�߰�λ
								Tx_Buffer[7]=V_max&0xFF;			//�Ͱ�λ
								Tx_Buffer[8]=(V_max>>8)&0xFF;	//�߰�λ
								Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
								HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);			//��������
								write_feedback(RxData,TxData);
								break;
							case 0x07:
								Tx_Buffer[3]=7;																											//����ѹд��
								Tx_Buffer[4]=SERVO_VIN_LIMIT_WRITE;
								V_max=RxData[6]+(RxData[7]<<8);
								if(V_min>=V_max)V_max=V_min+1;//��֤��С�����ѹС����������ѹ
								Tx_Buffer[5]=V_min&0xFF;			//�Ͱ�λ
								Tx_Buffer[6]=(V_min>>8)&0xFF;	//�߰�λ
								Tx_Buffer[7]=V_max&0xFF;			//�Ͱ�λ
								Tx_Buffer[8]=(V_max>>8)&0xFF;	//�߰�λ
								Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
								HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);
								write_feedback(RxData,TxData);
								break;
							case 0x11:
								Tx_Buffer[3]=7;																											//���ģʽд��
								Tx_Buffer[4]=SERVO_OR_MOTOR_MODE_WRITE;
								Tx_Buffer[5]=RxData[7];
								Tx_Buffer[6]=0;
								Tx_Buffer[7]=((unsigned short int)speed)&0xFF;											//ת���ٶ�ֵ
								Tx_Buffer[7]=(((unsigned short int)speed)>>8)&0xFF;
								Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
								HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);
								write_feedback(RxData,TxData);
								break;
							default:
								break;
						}
						break;
					case 2:	
						Tx_Buffer[2]=device_ID;																									//����צ��ID
						Tx_Buffer[3]=7;
						Tx_Buffer[4]=SERVO_MOVE_TIME_WRITE;																			//ģʽʱ��д��
						Tx_Buffer[5]=RxData[0];
						Tx_Buffer[6]=RxData[1];
						Tx_Buffer[7]=RxData[4];
						Tx_Buffer[8]=RxData[5];
						Tx_Buffer[Tx_Buffer[3]+2]=check_sum_pros(Tx_Buffer,Tx_Buffer[3]);
						HAL_HalfDuplex_EnableTransmitter(&huart1);
						HAL_UART_Transmit(&huart1,Tx_Buffer,Tx_Buffer[3]+3,0xffffffff);
						break;
					default:
							break;
				}
				can_send_msg(index,master_ID,can_ID, TxData, 8);														//���ͷ�������λ��
			}
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void command_pros(uint8_t index,uint8_t *Data,uint8_t len)
{
	TxData[0]=index;
	TxData[1]=len;
	switch(index)
	{
		case 0x01:
			TxData[7]=Data[5];
			break;
		case 0x02:
			TxData[7]=Data[5];
			break;
		case 0x03:
			TxData[6]=Data[5];	//�Ͱ�λ
			TxData[7]=Data[6];	//�߰�λ
			angel_min=Data[5]+(Data[6]<<8);//��С�Ƕȸ���
			angel_max=Data[7]+(Data[8]<<8);//���Ƕȸ���
			break;
		case 0x04:
			TxData[6]=Data[7];	//�Ͱ�λ
			TxData[7]=Data[8];	//�߰�λ
			angel_min=Data[5]+(Data[6]<<8);//��С�Ƕȸ���
			angel_max=Data[7]+(Data[8]<<8);//���Ƕȸ���
			break;
		case 0x05:
			TxData[6]=Data[5];
			TxData[7]=Data[6];
			break;
		case 0x06:
			TxData[6]=Data[5];
			TxData[7]=Data[6];
			V_min=Data[5]+(Data[6]<<8);		//��С��ѹ����
			V_max=Data[7]+(Data[8]<<8);		//����ѹ����
			break;
		case 0x07:
			TxData[6]=Data[7];
			TxData[7]=Data[8];
			V_min=Data[5]+(Data[6]<<8);		//��С��ѹ����
			V_max=Data[7]+(Data[8]<<8);		//����ѹ����
			break;
		case 0x08:
			TxData[6]=Data[5];
			TxData[7]=Data[6];
			break;
		case 0x09:
			TxData[7]=Data[5];
			break;
		case 0x10:
			TxData[7]=Data[5];
			break;
		case 0x11:
			TxData[7]=Data[5];
			speed=(signed short int)(Data[7]+(Data[8]<<8));	//�ٶ�ֵ����
			break;
	}
}
uint8_t check_sum_pros(uint8_t *Data,uint8_t len)		//У��ͼ���
{
	uint8_t i;
	uint16_t sum=0;
	for(i=2;i<len+2;i++)
	{
		sum+=Data[i];
	}
	return (~sum)&0xFF;
}
void clear(uint8_t *Data)													//��������
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		Data[i]=0;
	}
}
void write_feedback(uint8_t *Rx_Data,uint8_t *Tx_Data)		//���ݷ���
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		Tx_Data[i]=Rx_Data[i];
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
