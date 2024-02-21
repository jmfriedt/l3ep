#TARGET=RTC_I2Cjmf
#TARGET=RTC_I2Cjmf_alm
#TARGET=RTC_I2Cwakeup
#TARGET=RTC_I2Cwakeup_front
#TARGET=sd_init
#TARGET=RTC_I2Cjmf_libavr
TARGET=lm75

CPU=atmega32u4
CFLAGS=-mmcu=$(CPU) -Os -Wall -I../../include -I../../../../platforms/Atmega32/VirtualSerial/ -I../../../../platforms/Atmega32/lufa-LUFA-140928 -DF_USB=16000000UL -DF_CPU=16000000UL -Os -std=gnu99 

all:	$(TARGET).out

$(TARGET).out: $(TARGET).o
	avr-gcc $(CFLAGS) -L../../lib -L../../../../platforms/Atmega32/VirtualSerial -o $(TARGET).out $(TARGET).o -lLEEA -lVirtualSerial 
	avr-objdump -dSt $(TARGET).out > $(TARGET).lst
	avr-objcopy -O ihex $(TARGET).out $(TARGET).hex

$(TARGET).o: $(TARGET).c
	avr-gcc $(CFLAGS) -c $(TARGET).c

clean:
	rm *.o $(TARGET).out *hex *lst

flash: $(TARGET).out
	avrdude -c avr109 -b57600 -D -p $(CPU) -P /dev/ttyACM0 -e -U flash:w:$(TARGET).out 


$(EXEC).o: $(EXEC).c
	$(CC) $(CFLAGS) -c $(EXEC).c 

DFU=dfu-programmer

flash_dfu: $(TARGET).out
	$(DFU) $(CPU) erase
	$(DFU) $(CPU) flash $(TARGET).hex
	$(DFU) $(CPU) reset
