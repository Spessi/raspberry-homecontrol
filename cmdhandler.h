#ifndef CMDHANDLER__H
#define CMDHANDLER__H

typedef int (*cmd_fnct)(char*, int);

struct command {
	char* cmd;
	char* desc;
	int args;
	cmd_fnct function;
};

// Prototypes
void str_replace_cr(char*);
void str_replace_cr_space(char*);
void handle_cmds(char buffer[], int client_fd);
int cmd_help(char*, int);
int cmd_time(char*, int);

#endif