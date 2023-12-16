CC=gcc
#CC=avr-gcc -mmcu=atmega32u4

all: fft

main.o: main.c
	$(CC) -c main.c

maxqfft.o: maxqfft.c maxqfft.h
	$(CC) -c maxqfft.c
	
fft: main.o maxqfft.o
	$(CC) -o fft maxqfft.o main.o -lm

clean:
	rm *.o fft
