#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wiringPi.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "rf433.h"

void rf433_init(void) {
	pinMode(RF433_TX_PIN, OUTPUT);
	digitalWrite(RF433_TX_PIN, LOW);
	rf433_powerOff(1);
	rf433_powerOff(2);
	rf433_powerOff(3);
}

int cmd_rf(char* cmd, int client_fd) {
	char* pch;
	char args[10][35];
	int i = 0;
	unsigned char socket_nbr;
	
	pch = strtok(cmd, " ");
	while((pch != NULL) && (i<10)) {
		snprintf(args[i], 35, "%s", pch);
		pch = strtok(NULL, " ");
		i++;
	}
	
	if(strcmp(args[1], "?") == 0) {
		char txt_helplist[] = "rf set <socketnbr> <on/off> | example: rf set 1 on\r\n";
		send(client_fd, txt_helplist, strlen(txt_helplist), 0);
	}
	else if(strcmp(args[1], "set") == 0) {
		socket_nbr = (unsigned char) strtol(args[2], NULL, 10);
		if(socket_nbr > RF433_SOCKET_NBR_MAX)
			return -1;
	
		if(strcmp(args[3], "on") == 0) {
			rf433_powerOn(socket_nbr);
		}
		else if(strcmp(args[3], "off") == 0) {
			rf433_powerOff(socket_nbr);
		}
		else
			return -1;
	}
	else
		return -1;
	
	return 0;
}


/************************************************************************/
/* This method sends a logical high via the 433mhz rf transmitter       */
/************************************************************************/
void rf433_sendHigh(void) {
	digitalWrite(RF433_TX_PIN, HIGH);
	delayMicroseconds(960);
	digitalWrite(RF433_TX_PIN, LOW);
	delayMicroseconds(320);
	digitalWrite(RF433_TX_PIN, HIGH);
	delayMicroseconds(960);
	digitalWrite(RF433_TX_PIN, LOW);
	delayMicroseconds(320);
}

/************************************************************************/
/* This method sends a logical low via the 433mhz rf transmitter        */
/************************************************************************/
void rf433_sendLow(void) {
	digitalWrite(RF433_TX_PIN, HIGH);
	delayMicroseconds(320);
	digitalWrite(RF433_TX_PIN, LOW);
	delayMicroseconds(960);
	digitalWrite(RF433_TX_PIN, HIGH);
	delayMicroseconds(320);
	digitalWrite(RF433_TX_PIN, LOW);
	delayMicroseconds(960);
}

/************************************************************************/
/* This method sends a floating signal via the 433mhz rf transmitter    */
/************************************************************************/
void rf433_sendFloat(void) {
	digitalWrite(RF433_TX_PIN, HIGH);
	delayMicroseconds(320);
	digitalWrite(RF433_TX_PIN, LOW);
	delayMicroseconds(960);
	digitalWrite(RF433_TX_PIN, HIGH);
	delayMicroseconds(960);
	digitalWrite(RF433_TX_PIN, LOW);
	delayMicroseconds(320);
}

/************************************************************************/
/* This method sends the sync signal, which is necessary at the end     */
/************************************************************************/
void rf433_sendSync(void) {
	digitalWrite(RF433_TX_PIN, HIGH);
	delayMicroseconds(320);
	digitalWrite(RF433_TX_PIN, LOW);
	delay(10);
}


void rf433_powerOn(unsigned char nbr) {
	int i;
	
	for(i=0; i<10; i++) {
		// Send housecode
		rf433_sendHigh();
		rf433_sendFloat();
		rf433_sendFloat();
		rf433_sendFloat();
	
		// Send socketcode
		if(nbr == 1)
			rf433_sendHigh();
		rf433_sendFloat();
		if(nbr == 2)
			rf433_sendHigh();
		rf433_sendFloat();
		if(nbr == 3)
			rf433_sendHigh();
		rf433_sendLow();
	
		// Send statecode (on)
		rf433_sendLow();
		rf433_sendLow();
		rf433_sendHigh();
		rf433_sendLow();
	
		// Send synccode
		rf433_sendSync();
	}
}

void rf433_powerOff(unsigned char nbr) {
	int i;
	
	for(i=0; i<10; i++) {
		// Send housecode
		rf433_sendHigh();
		rf433_sendFloat();
		rf433_sendFloat();
		rf433_sendFloat();
	
		// Send socketcode
		if(nbr == 1)
			rf433_sendHigh();
		rf433_sendFloat();
		if(nbr == 2)
			rf433_sendHigh();
		rf433_sendFloat();
		if(nbr == 3)
			rf433_sendHigh();
		rf433_sendLow();
	
		// Send statecode (off)
		rf433_sendLow();
		rf433_sendLow();
		rf433_sendLow();
		rf433_sendHigh();
	
		// Send synccode
		rf433_sendSync();
	}
}