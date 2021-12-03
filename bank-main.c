#include "bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Default port and ip address are defined here

int main(int argc, char** argv){
	unsigned short port = 3000;
	char *ip = "127.0.0.1";
	int i;
	FILE *file;
	int auth_pos = -1; // returns the location in argv of an auth file. If not specified, it is -1.

	// Checks to make sure the parameters are valid 
	if(argc!=1 && argc!=3 && argc!=5){
		// printf("DEBUG: wrong number of arguments: expected val in {1,3,5} but recieved %d\n",argc);
		return 255;
	}else{
		if(argc==1){
			// no validation necessary
		}else if(argc==3){
			// validation of argv[1] and argv[2] necessary
			if(strcmp(argv[1],"-p")!=0 && strcmp(argv[1],"-s")!=0){
				// printf("DEBUG: second argument was an unexpected value: %s\n",argv[1]);
				return 255;
			}
			if(strcmp(argv[1],"-p")==0){
				// port argument recognized
				int port_num = atoi(argv[2]);
				if(port_num<1024||port_num>65535){
					// printf("DEBUG: port argument was an unexpected value: %s\n",argv[2]);
					return 255;
				}
				port = (unsigned short)port_num;
			}else{
				// specific auth file detected
				if((file = fopen(argv[2],"r"))){
					fclose(file);
					// printf("DEBUG: auth file was able to open: %s\n",argv[2]);
				}else{
					// printf("DEBUG: auth file was not able to open: %s\n",argv[2]);
					return 255;
				}
				auth_pos = 2;
			}
		}else {
			// validation of argv[1-4] necessary
			if(strcmp(argv[1],"-p")!=0 && strcmp(argv[1],"-s")!=0){
				// printf("DEBUG: second argument was an unexpected value: %s\n",argv[1]);
				return 255;
			}
			if(strcmp(argv[3],"-p")!=0 && strcmp(argv[3],"-s")!=0){
				// printf("DEBUG: fourth argument was an unexpected value: %s\n",argv[3]);
				return 255;
			}

			// validate argv[1] and argv[2]
			int first_was_port = 0; // 1 if the first argument was a port argument
			if(strcmp(argv[1],"-p")==0){
				// port argument recognized
				first_was_port = 1;
				int port_num = atoi(argv[2]);
				if(port_num<1024||port_num>65535){
					// printf("DEBUG: port argument was an unexpected value: %s\n",argv[2]);
					return 255;
				}
				port = (unsigned short)port_num;
			}else{
				// specific auth file detected
				if((file = fopen(argv[2],"r"))){
					fclose(file);
					// printf("DEBUG: auth file was able to open: %s\n",argv[2]);
				}else{
					// printf("DEBUG: auth file was not able to open: %s\n",argv[2]);
					return 255;
				}
				auth_pos = 2;}
			// validate argv[3] and argv[4]
			if(strcmp(argv[3],"-p")==0){
				// port argument recognized
				if(first_was_port==1){
					// printf("DEBUG: argv[3] was interpreted as a second port argument: %s\n",argv[3]);
					return 255;
				}
				int port_num = atoi(argv[4]);
				if(port_num<1024||port_num>65535){
					// printf("DEBUG: port argument was an unexpected value: %s\n",argv[4]);
					return 255;
				}
				port = (unsigned short)port_num;
			}else{
				// specific auth file detected
				if(first_was_port==0){
					// printf("DEBUG: argv[3] was interpreted as a second auth argument: %s\n",argv[3]);
					return 255;
				}
				if((file = fopen(argv[4],"r"))){
					fclose(file);
					// printf("DEBUG: auth file was able to open: %s\n",argv[4]);
				}else{
					// printf("DEBUG: auth file was not able to open: %s\n",argv[4]);
					return 255;
				}
				auth_pos = 4;}}
	}
	
	// print the values that we learned 
	printf("DEBUG: The port, as entered, is %i\n",(int)port);
	if(auth_pos!=-1){
		printf("DEBUG: The auth file is %s\n",argv[auth_pos]);
	}else{
		printf("DEBUG: The auth file is bank.auth\n");
	}
	


	/* no error checking is done on any of this. may need to modify this */
	Bank *b = bank_create("an auth file", ip, port);

	/* process each incoming client, one at a time, until complete */
	for(;;) {

		unsigned int len = sizeof(b->remote_addr);
		b->clientfd = accept(b->sockfd, (struct sockaddr*)&b->remote_addr, &len);
		if (b->clientfd < 0) {
			perror("error on accept call");
			exit(255);
		}

		/* okay, connected to bank/atm. Send/recv messages to/from the bank/atm. */
		char buffer[1024];
		bank_recv(b, buffer, sizeof(buffer));
		printf("bank received:  %s\n", buffer);
		strcpy(buffer, "money money money");
		bank_send(b, buffer, strlen(buffer)+1);


		/* when finished processing commands ...*/
		close(b->clientfd);
		b->clientfd = -1;
	}


	
	
	// Implement how atm protocol will work: sanitizing inputs and using different modes of operations

	return EXIT_SUCCESS;
}
