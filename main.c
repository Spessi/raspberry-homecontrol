#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <wiringPi.h>

#include "hardware/onewire/onewire.h"
#include "hardware/rf433/rf433.h"
#include "cmdhandler.h"


int main() {
	int server_port = 1337;
	pid_t PID;	// PID for processes
	char buffer[50];

	piHiPri(20);
	// Initialize WiringPi
	wiringPiSetup();
	
	// Initialize OneWire
	onewire_init();

	// Initialize RF433
	rf433_init();
	
	// Creating file descriptor for socket
	printf("\n");
	printf("/===========================================\\\n");
	printf("| Welcome to Raspberry Pi Home Control v0.1 |\n");
	printf("\\===========================================/\n\n");
	printf("Creating Socket ");
	int server_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(server_fd == -1)
		exit(EXIT_FAILURE);
	printf("[OK]\n");
		
	struct sockaddr_in server_addr;
	// Use TCP/IPv4 as protocoll
	server_addr.sin_family = AF_INET;
	// Set server to port 1337
	server_addr.sin_port = htons(server_port);
	// Allow access from every IP address
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	
	printf("Binding socket to port %d ", server_port);
	// Bind the local port 1337 to the file descriptor server_fd
	if(bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
		exit(EXIT_FAILURE);
	printf("[OK]\n");
	
	printf("Making ready to listen for new connections ");
	// Set linux kernel ready to listen for commands on the port (allowing up to 5 pending connections)
    if(listen(server_fd, 5) == -1)
		exit(EXIT_FAILURE);
	printf("[OK]\n\n");

	printf("Waiting for new connections...\n");
	// Main loop, accepting pending connections and do the work
	
    while(1) {
		struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
				
				// Create socket to the client and let the server socket still listen for new connections
				int client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_addr_len);
				if(client_fd == -1) 
					exit(EXIT_FAILURE);
				printf("New connection established.\n");

				switch(PID = fork()) {
					case -1: // Error creating new process
						close(client_fd);
						break;
					case 0:  // Child process
						while(1) {
							// Clearing buffer with zeros
							bzero(buffer, sizeof(buffer));
							
							// Read from stream
							int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
							
							if(bytes <= 0) { // Connection closed
								close(client_fd);
								printf("Connection closed\n");
								//Close child
								exit(0);
							} 
							else { // Successfully received, handle it!
								printf("[%s,%d] %s", inet_ntoa(client_addr.sin_addr), getpid(), buffer);
								handle_cmds(buffer, client_fd);
							}
						}
						break;
					default:  // Parent process
						close(client_fd);
						break;
				}
			}

        close(server_fd);

        return EXIT_SUCCESS;
}
