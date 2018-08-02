TARGET=usart_int_sleep
#TARGET=timer1_sleep
#TARGET=timer4_sleep

all:	$(TARGET).out

$(TARGET).out: $(TARGET).o
	avr-gcc -L../../lib -mmcu=atmega32u4 -g -Os -Wall -o $(TARGET).out $(TARGET).o -lLEEA
	avr-objdump -dSt $(TARGET).out > $(TARGET).lst

$(TARGET).o: $(TARGET).c
	avr-gcc -I../../include -mmcu=atmega32u4 -Os -Wall -c $(TARGET).c

clean:
	rm *.o $(TARGET).out


$(TARGET).hex: $(TARGET).out
	avr-objcopy -O ihex $(TARGET).out $(TARGET).hex

flash: $(TARGET).hex
	avrdude -c avr109 -b57600 -D -p atmega32u4 -P /dev/ttyACM0 -e -U flash:w:$(TARGET).hex
