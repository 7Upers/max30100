F_CPU = 16000000UL
CC = /usr/bin/avr-gcc
#-Wall enable all warnings
#-mcall-prologues convert code of functions (binnary will be smaller)
CFLAGS = -Os -Wall -mcall-prologues -mmcu=atmega328
#some defines
CFLAGS += -DF_CPU=$(F_CPU)
OBJ2HEX = /usr/bin/avr-objcopy
UISP = /usr/bin/avrdude
FLASHER = arduino
PORT = /dev/ttyUSB0
SPEED = 57600
TARGET = main

main.hex : $(TARGET).elf
	@echo 'converting'
	$(OBJ2HEX) -R .eeprom -O ihex $(TARGET).elf $(TARGET).hex

main.elf : $(TARGET).o i2chw.o uart.o
	@echo 'linking'
	$(CC) $(CFLAGS) -o $(TARGET).elf $(TARGET).o i2chw.o uart.o

uart.o : uart/uart.h uart/uart.c
	@echo 'uart'
	$(CC) $(CFLAGS) -c -o uart.o uart/uart.c

i2chw.o : i2chw/twimaster.c
	@echo 'i2c'
	$(CC) $(CFLAGS) -c -o i2chw.o i2chw/twimaster.c

main.o : $(TARGET).c
	@echo 'compilling'
	$(CC) $(CFLAGS) -c -o $(TARGET).o $(TARGET).c

prog : $(TARGET).hex
	@echo 'flashing'
	$(UISP) -F -V -c $(FLASHER) -P $(PORT) -b $(SPEED) -p m328 -U flash:w:$(TARGET).hex:a

clean :
	@echo 'cleaning'
	rm -f *.hex *.elf *.o

.SILENT: fuse
fuse:
	@echo -e 'get fuse bits\nhfuse\nlfuse\nefuse'
	$(UISP) -F -V -c $(FLASHER) -P $(PORT) -b $(SPEED) -p m328 -U hfuse:r:-:h -U lfuse:r:-:h -U efuse:r:-:h 2>/dev/null
