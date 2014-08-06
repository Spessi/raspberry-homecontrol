CC = gcc
CFLAGS = -Wall 
LDFLAGS = -lwiringPi
SRC = main.c cmdhandler.c hardware/rf433/rf433.c hardware/onewire/onewire.c
EXECUTABLE = homeserver

all: $(SRC)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SRC) $(LDFLAGS)
