#include "atm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Default port and ip address are defined here

int main(int argc, char** argv){
  	unsigned short port = 3000;
	char *ipAddr = "127.0.0.1";
	

	// welcome to parsing hell
	int i = 1;
	/*
	this is a bit complicated but work with me here
	recval_locs[0] corresponds to -s <auth> location in argv
	recval_locs[1] corresponds to -i <ip-addr> location in argv
	recval_locs[2] corresponds to -p <port> location in argv
	recval_locs[3] corresponds to -c <card-loc> location in argv
	recval_locs[4] corresponds to -a <account> location in argv
	recval_locs[5] corresponds to "active-operation" location in argv
	*/
	int recval_locs[] = [-1,-1,-1,-1,-1,-1];
	/*
	this is the type of active operation that we are dealing with here
	0 corresponds to a -n <balance> operation
	1 corresponds to a -d <amt> operation
	2 corresponds to a -w <amt> operation
	3 corresponds to a -g operation
	*/
	int active_operation = -1;
	while(i<argc){
		
	}



	ATM *atm = atm_create(ipAddr, port);

	/* send messages */

	char buffer[] = "Hello I am the atm and I would like to have money please";
	atm_send(atm, buffer, sizeof(buffer));
	atm_recv(atm, buffer, sizeof(buffer));
	printf("atm received %s\n", buffer);
	
	atm_free(atm);
	

	// Implement how atm protocol will work: sanitizing inputs and using different modes of operations

	return EXIT_SUCCESS;
}
