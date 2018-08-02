MCU=atmega32u4
REP=$(HOME)/enseignement/ufr/platforms/Atmega32/
#TARGET=gpio_int
#TARGET=timer_ovfirq
#TARGET=timer_irq
#TARGET=wdt
#TARGET=input_capture
TARGET=input_capture_sendai
#TARGET=switch_sendai
#TARGET=tmp
#TARGET=usart_int
#TARGET=gpr

all:	$(TARGET).hex

$(TARGET).hex: $(TARGET).out
	avr-objcopy -Oihex $(TARGET).out $(TARGET).hex  # for MS-Windows (winavr requires .hex)
	avr-objdump -dSt $(TARGET).out > $(TARGET).lst

$(TARGET).out: $(TARGET).o
	avr-gcc  -L$(REP)/VirtualSerial/ -mmcu=$(MCU) -o $(TARGET).out $(TARGET).o -lVirtualSerial

$(TARGET).o: $(TARGET).c
	avr-gcc -Wall -mmcu=$(MCU) -I$(REP)/VirtualSerial/ -I$(REP)/lufa-LUFA-140928/ -DF_USB=16000000UL \
	-std=gnu99 -Os -c $(TARGET).c
	avr-gcc -Wall -mmcu=$(MCU) -I$(REP)/VirtualSerial/ -I$(REP)/lufa-LUFA-140928/ -DF_USB=16000000UL \
	-std=gnu99 -Os -S $(TARGET).c

flash: $(TARGET).out
	avrdude -c avr109 -b57600 -D -p $(MCU) -P /dev/ttyACM0 -e -U flash:w:$(TARGET).out

clean:
	rm *.o $(TARGET).out
