CC=avr-gcc
CFLAGS=-mmcu=atmega32u4 -Os -Wall -I../../../../platforms/Atmega32/VirtualSerial/ -I../../../../platforms/Atmega32/lufa-LUFA-140928 -DF_USB=16000000UL -DF_CPU=16000000UL -std=gnu99
# EXEC=timer_ovf
# EXEC=blink
# EXEC=usart_string
# EXEC=lecture
EXEC=usart

all: $(EXEC).out

$(EXEC).out: $(EXEC).o
	$(CC) $(CFLAGS) -L../../../../platforms/Atmega32/VirtualSerial -o $(EXEC).out $(EXEC).o -lVirtualSerial

$(EXEC).o: $(EXEC).c
	$(CC) $(CFLAGS) -c $(EXEC).c 

flash: $(EXEC).out
	avrdude -c avr109 -b57600 -D -p atmega32u4 -P /dev/ttyACM0 -e -U flash:w:$(EXEC).out	

clean:
	rm *.o $(EXEC).out
