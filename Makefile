OBJS = main.c
NAME = main

all:
	make clean compile install

clean:
	rm $(NAME).hex $(NAME).o $(NAME).elf

install:
	avrdude -p m8 -c usbtiny -U flash:w:$(NAME).hex

compile:
	avr-gcc -g -Os -mmcu=atmega8 -c $(OBJS)
	avr-gcc -g -mmcu=atmega8 -o $(NAME).elf $(NAME).o
	avr-objcopy -j .text -j .data -O ihex $(NAME).elf $(NAME).hex
