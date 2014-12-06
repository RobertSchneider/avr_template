sudo apt-get install flex byacc bison gcc libusb libusb-dev libc6-dev binutils
wget "download.savannah.gnu.org/releases/avr-libc/avr-libc-1.8.1.tar.bz2"
tar xjvf avr-libc-1.8.1.tar.bz2
cd avr-libc-1.8.1
./configure --host=avr
make
sudo make install
cd ..

wget "download.savannah.gnu.org/releases/avrdude/avrdude-6.1.tar.gz"
tar -zxvf avrdude-6.1.tar.gz
cd avrdude-6.1

sudo apt-get install libusb-dev libpthread-stubs0-dev libelf-dev libusb-1.0 libftdi1 libhidapi-dev bison
./configure
make
sudo make install

