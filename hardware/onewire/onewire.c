#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include "onewire.h"



// Globals
ow_dev* ow_dev_list;



/**
* Used internally. Creates a new id-name-pair for a device,
* which will be added to the linked list
**/
ow_dev* mk_create_dev(char* id, char* name) {
	ow_dev* newDev = NULL;		// The new device

	// Create new device and set variables
	newDev = (ow_dev*)malloc(sizeof(*newDev));
	
	newDev->id = (char*) malloc(ID_LEN*sizeof(char*));
	newDev->name = (char*) malloc(NAME_LEN*sizeof(char*));
		
	strncpy(newDev->id, id, ID_LEN);
	strncpy(newDev->name, name, NAME_LEN);

	newDev->next = NULL;	// Because it's the last one

	return newDev;
}

/**
* This method sets an alias for a device id.
* Returns 0  if everything's fine
* Returns -1 if any argument is NULL
* Returns -2 if any argument is shorter than 1 char
* Returns -3 if the name already exists (must be unique!)
**/
int ow_set_alias(ow_dev** lst, char* id, char* name) {
	ow_dev* dev_iter = *lst;	// Point to first element
	
	if (id == NULL || name == NULL)
		return -1;

	// Check if id and name are halfway meaningful
	if ((strlen(id) < 1) || (strlen(name) < 1))
		return -2;


	if (dev_iter != NULL) {	// List isn't empty
		// Go trough each element (device), do some 
		// checks and store the last one in dev_iter
		while (1) {
			// Check if ID is already having an alias (for edit)
			if (strcmp(dev_iter->id, id) == 0)
				break;

			// Check if name is already used (must be unique!)
			if (strcmp(dev_iter->name, name) == 0)
				return -3;

			if (dev_iter->next == NULL) {
				// It's the last element, break!
				break;
			}
			else if (dev_iter->next != NULL) {
				// There's another element, go on!
				dev_iter = dev_iter->next;
			}
		}

		// Now dev_iter shows to the last element
		if (dev_iter->next == NULL) {
			// The id is not in the list, so add a new device
			dev_iter->next = mk_create_dev(id, name);
		}
		else {
			// The id is in the list, just change the name
			dev_iter->name = name;
		}
	}
	else {	// List is empty
		// Create the first entry
		*lst = mk_create_dev(id, name);
	}

	return 0;
}

/**
* This function is looking for an id in the linked list
* and deletes the entry
* Returns  0 if successfully deleted
* Returns -1 if lst is NULL
* Returns -2 if id is NULL
* Returns -3 if nothing was found to delete
**/
int ow_remove_id(ow_dev** lst, char* id) {
	if (lst == NULL)
		return -1;
	if (id == NULL)
		return -2;


	if (strcmp((*lst)->id, id) == 0) {
		ow_dev* temp = *lst;
		free(temp->id);
		free(temp->name);

		*lst = (*lst)->next;
		free(temp);
		return 0;
	}

	ow_dev* current = (*lst)->next;
	ow_dev* previous = *lst;
	while (current != NULL && previous != NULL) {
		if (strcmp(current->id, id) == 0) {
			ow_dev* temp = current;
			free(temp->id);
			free(temp->name);

			previous->next = current->next;
			free(temp);
			return 0;
		}
		previous = current;
		current = current->next;
	}
	return -3;
}

/**
* This function is looking for a name in the linked list
* and deletes the entry 
* Returns  0 if successfully deleted
* Returns -1 if lst is NULL
* Returns -2 if name is NULL
* Returns -3 if nothing was found to delete
**/
int ow_remove_alias(ow_dev** lst, char* name) {
	if (lst == NULL)
		return -1;
	if (name == NULL)
		return -2;


	if (strcmp((*lst)->name, name) == 0) {
		ow_dev* temp = *lst;
		free(temp->id);
		free(temp->name);

		*lst = (*lst)->next;
		free(temp);
		return 0;
	}

	ow_dev* current = (*lst)->next;
	ow_dev* previous = *lst;
	while (current != NULL && previous != NULL) {
		if (strcmp(current->name, name) == 0) {
			ow_dev* temp = current;
			free(temp->id);
			free(temp->name);

			previous->next = current->next;
			free(temp);
			return 0;
		}
		previous = current;
		current = current->next;
	}
	return -3;
}

/**
* This function delivers the alias (i.e. name) of a device id
* Returns "" if id is NULL or nothing was found,
* otherwise the alias
**/
char* ow_get_alias(ow_dev** lst, char* id) {
	ow_dev* dev_iter = *lst;

	if (id == NULL)
		return "";

	while (1) {
		if (strcmp(dev_iter->id, id) == 0) {
			return dev_iter->name;
		}

		if (dev_iter->next == NULL) {
			// It's the last element, break!
			break;
		}
		else if (dev_iter->next != NULL) {
			// There's another element, go on!
			dev_iter = dev_iter->next;
		}
	}

	return "";
}

/**
* This function delivers the id of a device alias
* Returns "" if name is NULL or nothing was found,
* otherwise the id
**/
char* ow_get_id(ow_dev** lst, char* name) {
	ow_dev* dev_iter = *lst;

	if (name == NULL)
		return "";

	while (1) {
		if (strcmp(dev_iter->name, name) == 0) {
			return dev_iter->id;
		}

		if (dev_iter->next == NULL) {
			// It's the last element, break!
			break;
		}
		else if (dev_iter->next != NULL) {
			// There's another element, go on!
			dev_iter = dev_iter->next;
		}
	}

	return "";
}



// OLD CODE

/**
* This function reads the configuration file and
* adds the device name with its id to the ow_list
**/
int onewire_name_init() {
	char id[ID_LEN];
	char name[NAME_LEN];
	
	// First, set ow_dev_list to a defined state (NULL)
	ow_dev_list = NULL;
	
	FILE* pOWNames = fopen(NAMES_CONF, "r");
	if(pOWNames != NULL) {
		while(fscanf(pOWNames, "%24s %49s", id, name) > 0) { // read each device in NAMES_CONF until EOF
			if((id != NULL) && (name != NULL))
				ow_set_alias(&ow_dev_list, id, name);
		}
		fclose(pOWNames);
		return 0;
	}
	return -1;
}

/**
* This function saves the id-name-pairs from ow_list
* in the NAME_CONF file
**/
int onewire_name_save(ow_dev** lst) {
	ow_dev* dev_iter;
	
	FILE* pOWNames = fopen(NAMES_CONF, "w");
	if(pOWNames != NULL) {
		while (1) {
			fprintf(pOWNames, "%s %s\r\n", dev_iter->id, dev_iter->name);

			if (dev_iter->next == NULL) {
				// It's the last element, break!
				break;
			}
			else if (dev_iter->next != NULL) {
				// There's another element, go on!
				dev_iter = dev_iter->next;
			}
		}
		fclose(pOWNames);
		
		return 0;
	}
	
	return -1;
}

/**
* Needs to be called on application start
**/
void onewire_init() {
	onewire_name_init();
}

/**
* Command handler for the one-wire part
**/
int cmd_ow(char* cmd, int client_fd) {
	char* pch;
	int i = 0;
	FILE* pOWBus;
	char buffer[50];

	char* args[MAX_ARGS]; // max 10 arguments
	memset(args, 0, MAX_ARGS*sizeof(char*));	// Set every element to NULL
	
	// Initialize strtok and get first token
	pch = strtok(cmd, " ");
	while((pch != NULL) && (i<MAX_ARGS)) {
		// Copy token (i.e. argument) to args list
		args[i] = (char*) malloc(sizeof(char)*MAX_ARG_LEN);
		snprintf(args[i], MAX_ARG_LEN, "%s", pch);
		
		// Get next token
		pch = strtok(NULL, " ");
		
		// Increment argument list
		i++;
	}

	// 0. argument is already handled in cmdhandler.c
	// So let's check the 1. argument
	if(strcmp(args[1], "?") == 0) {	// Display help
		char txt_helplist[] =	"ow list\r\n"
								"ow temp <name> | ow temp <id>\r\n"
								"ow alias set <id> <name> | example: ow alias 10-000801b22d7c Outdoor\r\n"
								"ow alias get <id> | returns the sensors alias if it's set\r\n"
								"ow alias get <name> | returns the sensors id if alias exists\r\n";
		send(client_fd, txt_helplist, strlen(txt_helplist), 0);
	}
	else if(strcmp(args[1], "list") == 0) {
		pOWBus = fopen("/sys/bus/w1/devices/w1_bus_master1/w1_master_slaves", "r");
		if(pOWBus != NULL) {
			while(fgets(buffer, 50, pOWBus) != NULL) {
				send(client_fd, buffer, strlen(buffer), 0);
				send(client_fd, "\r", strlen("\r"), 0);
			}
			fclose(pOWBus);
		}
		else
			goto fail;
	}
	else if(strcmp(args[1], "temp") == 0) {
		unsigned char crc_ok = 0;
		float temperature;
		char addr[75];
		char id[ID_LEN];
		
		if(args[2] == NULL)
			goto fail;

		// Let's check if we have an alias
		strncpy(id, ow_get_id(&ow_dev_list, args[2]), ID_LEN);
		if(strcmp(id, "") == 0)
			strncpy(id, args[2], ID_LEN);
			
		// And check if the id is available
		pOWBus = fopen("/sys/bus/w1/devices/w1_bus_master1/w1_master_slaves", "r");
		if(pOWBus != NULL) {
			int match = 0;
			while(fscanf(pOWBus, "%s", buffer) > 0) {
				if(strcmp(buffer, id) == 0) {
					match = 1;
					break;
				}
			}
			if(match == 0)
				goto fail;
				
			fclose(pOWBus);
		}
		else
			goto fail;
		
		// Now open the sensors value file
		snprintf(addr, 75, "/sys/bus/w1/devices/w1_bus_master1/%s/w1_slave", id);
		printf("Opening %s\n", addr);
		pOWBus = fopen(addr, "r");
		if(pOWBus != NULL) {
			while(fscanf(pOWBus, "%s", buffer) > 0) {
				if(strcmp(buffer, "YES") == 0)
					crc_ok = 1;
			}
			fclose(pOWBus);
			
			// Read out temperature
			if(crc_ok == 1) {
				memmove(buffer, buffer + 2, strlen(buffer));
				temperature = (float)strtol(buffer, NULL, 10) / 1000.0f;
				
				snprintf(buffer, 15, "%.2f°C\r\n", temperature);
				send(client_fd, buffer, strlen(buffer), 0);
			}
			else 
				goto fail;
		}
		else
			goto fail;
	}
	else {
		goto fail;
	}	
	
	for(i=0;i<MAX_ARGS;i++) {
		free(args[i]);
	}
	return 0;

	fail:
	for(i=0;i<MAX_ARGS;i++) {
		free(args[i]);
	}
	return -1;
}
