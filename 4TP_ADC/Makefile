#TARGET=adc_int2
#TARGET=adc_poll_fast
#TARGET=adc_poll
#TARGET=adc_timer4
#TARGET=adc_timer4internal
#TARGET=adc_timer1internal
#TARGET=adc_timer4_rapide
#TARGET=adc_timer4_sleep
#TARGET=adc_timer4_commente
#TARGET=adc_int
TARGET=fmspic
#TARGET=adc_timer1internal_7seg
#TARGET=adc_timer1internal_to_pwm

all:	$(TARGET).hex

$(TARGET).out: $(TARGET).o affiche.o
	avr-gcc -L../../../../platforms/Atmega32/VirtualSerial/ -mmcu=atmega32u4 -g -Os -o $(TARGET).out $(TARGET).o affiche.o -lVirtualSerial
	avr-objdump -dSt $(TARGET).out > $(TARGET).lst

$(TARGET).hex: $(TARGET).out
	avr-objcopy -O binary $(TARGET).out $(TARGET).bin
	avr-objcopy -O ihex $(TARGET).out $(TARGET).hex

affiche.o: affiche.c
	avr-gcc -mmcu=atmega32u4 -Os -Wall -c affiche.c

$(TARGET).o: $(TARGET).c
	avr-gcc -I../../../../platforms/Atmega32/VirtualSerial/ \
        -I../../../../platforms/Atmega32/lufa-LUFA-140928 -mmcu=atmega32u4 \
        -DF_USB=16000000UL -std=gnu99 -Os -Wall -c $(TARGET).c

clean:
	rm *.o $(TARGET).out *.lst *aux *log

flash: $(TARGET).hex
	avrdude -c avr109 -b57600 -D -p atmega32u4 -P /dev/ttyACM0 -e -U flash:w:$(TARGET).hex
