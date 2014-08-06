#ifndef ONEWIRE__H
#define ONEWIRE__H

#define NAMES_CONF	"names.conf"	// Name of the name-id database
#define MAX_ARGS	10
#define MAX_ARG_LEN	30
#define ID_LEN		25
#define NAME_LEN	50

/* Struct for linked list */
struct s_ow_device {
	char* id;
	char* name;

	struct s_ow_device* next;
};
typedef struct s_ow_device ow_dev;



/* prototypes */
void onewire_init();
int cmd_ow(char*, int);

#endif
