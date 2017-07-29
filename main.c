#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "i2chw/i2cmaster.h"
#include "uart/uart.h"

#define LED PB5
#define MAX30100 0xAE

int main(void)
{
	//setup led output
	DDRB = (1<<LED);

	uart_init(MYUBRR);

	sei();
	i2c_init();
	printf("I2C bus inited\r\n");

	unsigned char ret;

	while (1)
	{
		//led turn on
		PORTB |= (1<<LED);

		i2c_start_wait(MAX30100 | I2C_WRITE);

//		if ( ret == 0 )
//		{
//			printf("start success\n");
			i2c_write(0xFF); //part id register
			i2c_rep_start(MAX30100 | I2C_READ);
			ret = i2c_read(0);
			printf("part id = 0x%02x\n", ret);
//		}
//		else
//		{
//			printf("start fail\n");
//		}

		i2c_stop();

		//led turn off
		PORTB &= ~(1<<LED);

		_delay_ms(1000);
	}
}