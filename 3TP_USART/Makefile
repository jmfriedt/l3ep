MCU=atmega32u4
TARGET=usart_int

all:	$(TARGET).hex

$(TARGET).hex: $(TARGET).out
	avr-objcopy -Oihex $(TARGET).out $(TARGET).hex  # for MS-Windows (winavr requires .hex)
	avr-objdump -dSt $(TARGET).out > $(TARGET).lst

$(TARGET).out: $(TARGET).o
	avr-gcc -mmcu=$(MCU) -o $(TARGET).out $(TARGET).o

$(TARGET).o: $(TARGET).c
	avr-gcc -Wall -g -mmcu=$(MCU) -Os -c $(TARGET).c

flash: $(TARGET).out
	avrdude -c avr109 -b57600 -D -p $(MCU) -P /dev/ttyACM0 -e -U flash:w:$(TARGET).out

clean:
	rm *.o $(TARGET).out
