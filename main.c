#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "i2chw/i2cmaster.h"
#include "uart/uart.h"

#define LED PB5
#define MAX30100 0xAE
#define MAX30100_R MAX30100 | I2C_READ
#define MAX30100_W MAX30100 | I2C_WRITE
#define MAX30100_PART_ID 0xFF
#define MAX30100_REV_ID 0xFE
#define MAX30100_MODE 0x06
#define MAX30100_TINT 0x16
#define MAX30100_TFRAC 0x17
#define MAX30100_EINT 0x01

ISR(INT0_vect)
{
	cli();

	i2c_start(MAX30100_W);
	i2c_write(0x00);
	i2c_rep_start(MAX30100_R);
	uint8_t ret = i2c_readNak();
	printf("current state 0x%02x\r\n", ret);
	i2c_stop();

//	printf("temp is ready to read\r\n");

	i2c_start(MAX30100_W);
	i2c_write(MAX30100_TINT);
	i2c_rep_start(MAX30100_R);
	ret = i2c_read(0);
	printf("curr temp int 0x%02x\r\n", ret);

	i2c_rep_start(MAX30100_W);
	i2c_write(MAX30100_TFRAC);
	i2c_rep_start(MAX30100_R);
	ret = i2c_read(0);
	printf("curr temp frac 0x%02x\r\n", ret);
	i2c_stop();
}

int main(void)
{
	//setup led output
	DDRB = (1<<LED);

	uart_init(MYUBRR);

	//Set Up INT0
	//input
	DDRD &= ~(1<<PD2);
	PORTD |= (1<<PD2);	//pullup resistor
	EICRA |= (1<<ISC01);	//The falling edge of INT0 generates an interrupt request.
	EIMSK |= (1<<INT0);	//Enable INT0


	i2c_init();
	printf("I2C bus inited\r\n");

	uint8_t ret = 1;
	uint8_t max30100 = 0;

	ret = i2c_start(MAX30100_W);

	if ( ret == 0 )
	{
		max30100 = 1;
		printf("max30100 connected\r\n");
		i2c_write(MAX30100_PART_ID);
		i2c_rep_start(MAX30100_R);
		ret = i2c_read(0);
		printf("part id = 0x%02x\r\n", ret);

		i2c_rep_start(MAX30100_W);
		i2c_write(MAX30100_REV_ID);
		i2c_rep_start(MAX30100_R);
		ret = i2c_read(0);
		printf("revision id = 0x%02x\r\n", ret);
		i2c_stop();

		//configure
		i2c_start(MAX30100_W);
		i2c_write(MAX30100_EINT);
		i2c_write(0x40); //temp interrupt
		i2c_stop();

		i2c_start(MAX30100_W);
		i2c_write(MAX30100_EINT);
		i2c_rep_start(MAX30100_R);
		ret = i2c_readNak();
		i2c_stop();

		if ( ret&0x40 )
		{
			printf("temp interrupt enabled\r\n");
		}
		else
		{
			printf("temp interrupt disabled\r\n");
		}
	}
	else
	{
		printf("max30100 not answer\n");
	}

	PORTB &= ~(1<<LED);

	sei();

	while (1)
	{

		if ( max30100 == 1 )
		{
			//led toggle
			PORTB ^= (1<<LED);

			//start temp convert
			i2c_start(MAX30100_W);
			i2c_write(MAX30100_MODE);
			i2c_write(0x08|0x02); //HR
			i2c_stop();
			printf("start temp conversion\r\n");
		}

		_delay_ms(5000);
	}
}