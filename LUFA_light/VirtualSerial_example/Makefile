# MCU     = atmega32u2
MCU	= atmega32u4
TARGET	= VirtualSerial

all: $(TARGET).elf

$(TARGET).elf: $(TARGET).o
	avr-gcc -mmcu=$(MCU) -L../VirtualSerial_lib $(TARGET).o -o $(TARGET).elf -lVirtualSerial

VirtualSerial.o: VirtualSerial.c
	avr-gcc -c -mmcu=$(MCU) -Wall -I. -I../lufa-LUFA-140928/ -DF_USB=16000000UL -DF_CPU=16000000UL -Os -std=gnu99  $(TARGET).c

clean:
	rm *.o *.elf

DFU=dfu-programmer

flash_dfu:
	@$(DFU) $(MCU) erase
	@$(DFU) $(MCU) flash $(TARGET).hex
	@$(DFU) $(MCU) reset

flash_109: $(TARGET).elf
	avrdude -c avr109 -b57600 -D -p $(MCU) -P /dev/ttyACM0 -e -U flash:w:$(TARGET).elf

