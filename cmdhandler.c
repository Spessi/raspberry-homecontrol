#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "cmdhandler.h"
#include "hardware/rf433/rf433.h"
#include "hardware/onewire/onewire.h"

struct command cmd_list[] = {
	{ "?", "Show all available commands", 0, cmd_help },
	{ "time", "Current time", 0, cmd_time },
	{ "ow", "List of all 1-Wire commands", 0, cmd_ow },
	{ "rf", "List of all RF433 commands", 0, cmd_rf }
};

#define ARRAY_SIZE(X) (sizeof(X) / sizeof(*(X)))

void str_replace_cr(char* src) {
	char* ptr = src;

	while (*ptr++ != '\0') {
		if ((*ptr == '\r') || (*ptr == '\n')) {
			*ptr = '\0';
			break;
		}
	}
}

void str_replace_cr_space(char* src) {
	char* ptr = src;

	while (*ptr++ != '\0') {
		if ((*ptr == '\r') || (*ptr == '\n') || (*ptr == ' ')) {
			*ptr = '\0';
			break;
		}
	}
}

void handle_cmds(char buffer[], int client_fd) {
	char tmp[50];
	int i = 0;
	
	// Filter out only the first word...
	strncpy(tmp, buffer, 50);
	str_replace_cr_space(tmp);
	// ... and compare the temp buffer with known commands
	while(strcmp(cmd_list[i].cmd, tmp) != 0) {
		if(i >= ARRAY_SIZE(cmd_list)-1) {
			i = -1;
			break;
		}
		i++;
	}
	
	// Remove carriage returns from buffer
	str_replace_cr(buffer);	
	if(i >= 0) {
		//Fire up command
		if(cmd_list[i].function(buffer, client_fd) == 0)
			send(client_fd, "ACK\r\n", 5, 0);
		else
			send(client_fd, "NACK\r\n", 6, 0);
	}
	else {
		// Command wasn't found
		char txt_unknown[] = "Unknown command. Send ? for a command list.\r\n";
		send(client_fd, txt_unknown, sizeof(txt_unknown), 0);
	}
}

int cmd_help(char* cmd, int client_fd) {
	int i = 0;
	
	for(i=0; i < ARRAY_SIZE(cmd_list); i++) {
		char line[50];
		snprintf(line, 50, "%s - %s\r\n", cmd_list[i].cmd, cmd_list[i].desc);
		if(send(client_fd, line, strlen(line), 0) == -1)
			return 1;
	}
	
	return 0;
}

int cmd_time(char* cmd, int client_fd) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char line[30];
	
	snprintf(line, 30, "now: %04d-%02d-%02d %02d:%02d:%02d\r\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	if(send(client_fd, line, strlen(line), 0) == -1)
		return 1;
		
	return 0;
}