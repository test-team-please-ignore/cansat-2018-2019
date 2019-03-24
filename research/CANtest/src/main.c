#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"

#include <mavlink/granum/mavlink.h>
#include "canmavlink.h"

volatile uint32_t msTicks;

void SysTick_Handler (void)
{
  msTicks++;
}

//-------------------------------
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

int main(int argc, char* argv[])
{
	volatile int err = 0, mb = 0;

	SystemInit();
	trace_printf("System clock: %u Hz\n", SystemCoreClock);
	SysTick_Config(SystemCoreClock/1000);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

	GPIO_InitTypeDef gpio_init;
	GPIO_StructInit(&gpio_init);

	gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init.GPIO_Pin = GPIO_Pin_9;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_init);

	gpio_init.GPIO_Mode = GPIO_Mode_IPU;
	gpio_init.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &gpio_init);

	CAN_InitTypeDef can_init_structure;
	CAN_StructInit(&can_init_structure);

	can_init_structure.CAN_NART = ENABLE;
	can_init_structure.CAN_SJW = CAN_SJW_1tq;
	can_init_structure.CAN_BS1 = CAN_BS1_4tq;
	can_init_structure.CAN_BS2 = CAN_BS2_4tq;
	can_init_structure.CAN_Prescaler = 16 * 25;
	can_init_structure.CAN_Mode = CAN_Mode_Normal;

	err = CAN_Init(CAN1, &can_init_structure);

	CanTxMsg tx_msg = {
		.StdId = 0x1a1,
		.IDE = CAN_ID_STD,
		.RTR = CAN_RTR_Data,
		.DLC = 8,
		.Data = {0x13, 0x00, 'H', 'E', 'L', 'L', 'O', '\n'}
	};

	while(1)
	{
		mb = CAN_Transmit(CAN1, &tx_msg);

		Delay(1000);

		do {
			err = CAN_TransmitStatus(CAN1, mb);
		} while(err == 2);
	}
}
