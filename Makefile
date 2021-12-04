CC = gcc
CFLAGS = -fno-stack-protector -Wall -Wextra -Iutil -Iatm -Ibank -Irouter -I.
LDFLAGS=-lcrypto -lssl


all: atm bank 

atm: atm-main.o atm.o net.o -lcrypto
bank: bank-main.o bank.o net.o -lcrypto

clean:
	rm -f atm bank *.o
