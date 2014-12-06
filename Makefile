OBJS = main.c
NAME = main

all:
	avr-gcc -g -Os -mmcu=atmega8 -c $(OBJS)
	avr-gcc -g -mmcu=atmega8 -o $(NAME).elf $(NAME).o
	avr-objcopy -j .text -j .data -O ihex $(NAME).elf $(NAME).hex

clean:
	rm $(NAME).hex $(NAME).o $(NAME).elf

install:
	avrdude -p m8 -c usbtiny -U flash:w:$(NAME).hex
