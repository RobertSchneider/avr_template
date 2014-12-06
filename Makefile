all:
	avrdude -p m8 -c usbtiny -U flash:w:main.hex
