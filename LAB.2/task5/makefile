# SETTING
# /dev/spidev0.0	: SPI-PORT
# /dev/ttyAMA0		: UART-PORT
# gpio				: GPIO Programmer
MCU			= atmega328p
TARGET 		= main
SOURCES		= main.c init.h uart.h
PROGRAMMER	= gpio
PORT 		= /dev/ttyAMA0
BAUD 		= 9600
FORMAT		= ihex

# COMPILER OPTIONS
# -g		: Debugging Information
# -c		: 
# -Werror	: Error Level
# -Wall		: Warning Level
# -O*		: Optimization Level
CC			= avr-gcc
OBJECTS 	= $(TARGET).o
OPTIMIZE	= s
CFLAGS 		= -g -c -Werror -Wall -O$(OPTIMIZE)

# LINKER OPTIONS
LDFLAGS		= -Wl,-gc-sections -Wl,-relax

# MAKE COMMANDS
all : clean hex program
hex : $(TARGET).hex
$(TARGET).hex : $(TARGET).elf
	avr-objcopy -O $(FORMAT) $< $@
$(TARGET).elf : $(OBJECTS)
	$(CC) $(LDFLAGS) -mmcu=$(MCU) $(OBJECTS) -o $(TARGET).elf
.c.o : 
	$(CC) $(CFLAGS) -mmcu=$(MCU) $< -o $@
size :
	avr-size --mcu=$(MCU) -C $(TARGET).elf
program :
	avrdude \
		-p $(MCU) \
		-P $(PORT) \
		-b $(BAUD) \
		-c $(PROGRAMMER) \
		-e \
		-U flash:w:$(TARGET).hex:a
clean :
	rm -rf *.o *.elf *.hex
