OBJS = main.c
NAME = main

all:
	make compile 
	make install

clean:
	sudo rm $(NAME).hex $(NAME).o $(NAME).elf

install:
	sudo avrdude -p m8 -P /dev/ttyUSB0 -c avr910 -U flash:w:$(NAME).hex

compile:
	sudo avr-gcc -g -Os -mmcu=atmega8 -c main.c -std=c99 -I/usr/local/avr/include/ -Wall
	avr-gcc -g -mmcu=atmega8 -o $(NAME).elf $(NAME).o
	avr-objcopy -j .text -j .data -O ihex $(NAME).elf $(NAME).hex
