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
		printf("DEBUG: wrong number of arguments: expected val in {1,3,5} but recieved %d\n",argc);
		exit(255);
	}else{
		if(argc==1){
			// no validation necessary
		}else if(argc==3){
			// validation of argv[1] and argv[2] necessary
			if(strcmp(argv[1],"-p")!=0 && strcmp(argv[1],"-s")!=0){
				printf("DEBUG: second argument was an unexpected value: %s\n",argv[1]);
				exit(255);
			}
			if(strcmp(argv[1],"-p")==0){
				// port argument recognized
				int port_num = atoi(argv[2]);
				if(port_num<1024||port_num>65535){
					printf("DEBUG: port argument was an unexpected value: %s\n",argv[2]);
					exit(255);
				}
				port = (unsigned short)port_num;
			}else{
				// specific auth file detected
				auth_pos = 2;
			}
		}else {
			// validation of argv[1-4] necessary
			if(strcmp(argv[1],"-p")!=0 && strcmp(argv[1],"-s")!=0){
				printf("DEBUG: second argument was an unexpected value: %s\n",argv[1]);
				exit(255);
			}
			if(strcmp(argv[3],"-p")!=0 && strcmp(argv[3],"-s")!=0){
				printf("DEBUG: fourth argument was an unexpected value: %s\n",argv[3]);
				exit(255);
			}

			// validate argv[1] and argv[2]
			int first_was_port = 0; // 1 if the first argument was a port argument
			if(strcmp(argv[1],"-p")==0){
				// port argument recognized
				first_was_port = 1;
				int port_num = atoi(argv[2]);
				if(port_num<1024||port_num>65535){
					printf("DEBUG: port argument was an unexpected value: %s\n",argv[2]);
					exit(255);
				}
				port = (unsigned short)port_num;
			}else{
				// specific auth file detected
				auth_pos = 2;}
			// validate argv[3] and argv[4]
			if(strcmp(argv[3],"-p")==0){
				// port argument recognized
				if(first_was_port==1){
					printf("DEBUG: argv[3] was interpreted as a second port argument: %s\n",argv[3]);
					exit(255);
				}
				int port_num = atoi(argv[4]);
				if(port_num<1024||port_num>65535){
					printf("DEBUG: port argument was an unexpected value: %s\n",argv[4]);
					exit(255);
				}
				port = (unsigned short)port_num;
			}else{
				// specific auth file detected
				if(first_was_port==0){
					printf("DEBUG: argv[3] was interpreted as a second auth argument: %s\n",argv[3]);
					exit(255);
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
	
	Bank *b = bank_create((auth_pos!=-1)?argv[auth_pos]:"bank.auth", ip, port);

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
		/* Buffer message format:
		0 account name (122 characters)
		1 card file value (32 characters)
		2 mode of operation (1 character)
		3 value of operation (13 characters)
		4 anti-repeat attack value (32 characters)
		*/
		char sent_account[123];
		memset(sent_account,'\0',sizeof(sent_account));
		strncpy(sent_account,buffer,122);
		printf("DEBUG: ATM sent an account with account name of %s.\n",sent_account);
		// printf("DEBUG: Remaining string is %s.\n",&(buffer[122]));
		char sent_card_value[33];
		memset(sent_card_value,'\0',sizeof(sent_card_value));
		strncpy(sent_card_value,&(buffer[122]),32);
		printf("DEBUG: ATM sent a card value of %s.\n",sent_card_value);
		// printf("DEBUG: Remaining string is %s.\n",&(buffer[122+32]));
		char sent_mode_of_operation[3];
		memset(sent_mode_of_operation,'\0',sizeof(sent_mode_of_operation));
		strncpy(sent_mode_of_operation,&(buffer[122+32]),2);
		printf("DEBUG: ATM sent a mode of operation of %s.\n",sent_mode_of_operation);
		// printf("DEBUG: Remaining string is %s.\n",&(buffer[122+32+2]));
		char sent_value_of_operation[15];
		memset(sent_value_of_operation,'\0',sizeof(sent_value_of_operation));
		strncpy(sent_value_of_operation,&(buffer[122+32+2]),13);
		printf("DEBUG: ATM sent a value of operation of %s.\n",sent_value_of_operation);
		printf("DEBUG: Remaining string is %s.\n",&(buffer[122+32+1+13]));


		// printf("bank received:  %s\n", buffer);
		strcpy(buffer, "money money money");
		bank_send(b, buffer, strlen(buffer)+1);


		/* when finished processing commands ...*/
		close(b->clientfd);
		b->clientfd = -1;
	}


	
	
	// Implement how atm protocol will work: sanitizing inputs and using different modes of operations

	return EXIT_SUCCESS;
}
