#include <stdio.h>
#include <pigpio.h>
#include <sx1268.h>

#define RXBUFFLEN 1024

#define CSPIN	8
#define BUSYPIN	90 //FIXME redefine
#define IRQPIN	91

int main()
{
	int  err;
	sx1268_t radio;
	uint8_t rxbuff[RXBUFFLEN];
	uint8_t tmpbuff[RXBUFFLEN];

	printf("Ouuff... You did it!\n");

	if (( err = gpioInitialise() ) < 0)
	{
		printf("pigpio initialisation failed with code %d! Exitting\n", err);
		return 0;
	}

	int spihandle = spiOpen(0,10000, (1 << 5)); //spi0.0@10kHz, self-controlling CS

	gpioSetMode(CSPIN, PI_OUTPUT);
	gpioWrite(CSPIN, 1);

	gpioSetMode(BUSYPIN, PI_INPUT);

	gpioSetMode(IRQPIN, PI_INPUT);
	gpioSetISRFuncEx(IRQPIN, RISING_EDGE, 0, irqcallback, &radio);

	sx1268_rpi_t radio_specific =
	{
		.bus_handle = spihandle,
		.busy_pin = BUSYPIN,
		.cs_pin = CSPIN
	};
	sx1268_struct_init(&radio, rxbuff, RXBUFFLEN, NULL, 0);

	sx1268_init(&radio);

	while(1) {
		if( RXLEN(radio) != 0)
		{
			sx1268_receive(&radio, tmpbuff, RXLEN(radio));
		}

		printf("RUNNING!!!\n");
	}


	return 0;
}

void irqcallback(int gpio, int level, uint32_t tick, void * userdata)
{
	if(gpio == IRQPIN && level == RISING_EDGE)
		sx1268_event(userdata);
}