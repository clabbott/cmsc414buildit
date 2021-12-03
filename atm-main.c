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
	/* this is a bit complicated but work with me here
	recval_locs[0] corresponds to -s <auth> location in argv
	recval_locs[1] corresponds to -i <ip-addr> location in argv
	recval_locs[2] corresponds to -p <port> location in argv
	recval_locs[3] corresponds to -c <card-loc> location in argv
	recval_locs[4] corresponds to -a <account> location in argv
	recval_locs[5] corresponds to "mode of operation" location in argv
	*/
	int recval_locs[] = {-1,-1,-1,-1,-1,-1};
	/*
	this is the mode of operation that we are dealing with here
	0 corresponds to a -n <balance> operation
	1 corresponds to a -d <amt> operation
	2 corresponds to a -w <amt> operation
	3 corresponds to a -g operation
	*/
	int mode_of_operation = -1;
	while(i<argc){
		if(strcmp(argv[i],"-s")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(recval_locs[0]!=-1){
				printf("DEBUG: Duplicate %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				recval_locs[0] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-i")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(recval_locs[1]!=-1){
				printf("DEBUG: Duplicate %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				recval_locs[1] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-p")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(recval_locs[2]!=-1){
				printf("DEBUG: Duplicate %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				recval_locs[2] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-c")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(recval_locs[3]!=-1){
				printf("DEBUG: Duplicate %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				recval_locs[3] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-a")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(recval_locs[4]!=-1){
				printf("DEBUG: Duplicate %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				recval_locs[4] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-n")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(mode_of_operation!=-1){
				printf("DEBUG: Duplicate mode of operation argument with a mode of %s detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				mode_of_operation = 0;
				recval_locs[5] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-d")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(mode_of_operation!=-1){
				printf("DEBUG: Duplicate mode of operation argument with a mode of %s detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				mode_of_operation = 1;
				recval_locs[5] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-w")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(mode_of_operation!=-1){
				printf("DEBUG: Duplicate mode of operation argument with a mode of %s detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				mode_of_operation = 2;
				recval_locs[5] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-g")==0){
			// printf("DEBUG: %s argument detected at %d\n",argv[i],i);
			if(mode_of_operation!=-1){
				printf("DEBUG: Duplicate mode of operation argument with a mode of %s detected at %d\n",argv[i],i);
				exit(255);
			}else{
				mode_of_operation = 3;
				recval_locs[5] = i;
			}
			i+=1;
		}else{
			printf("DEBUG: Argument at %d was invalid. Value is %s\n",i,argv[i]);
			exit(255);
		}
	}

	for(int i=0;i<6;i++){
		printf("DEBUG: recval_locs[%d] value is %d\n",i,recval_locs[i]);
	}
	printf("DEBUG: mode_of_operation = %d\n",mode_of_operation);

	// Time to validate the values and determine what's going on now 
	/* this is a bit complicated but work with me here
	recval_locs[0] corresponds to -s <auth> location in argv
	recval_locs[1] corresponds to -i <ip-addr> location in argv
	recval_locs[2] corresponds to -p <port> location in argv
	recval_locs[3] corresponds to -c <card-loc> location in argv
	recval_locs[4] corresponds to -a <account> location in argv
	recval_locs[5] corresponds to "mode of operation" location in argv
	*/
	char* auth_file_name = (recval_locs[0]==-1 ? "bank.auth" : argv[recval_locs[0]+1]);
	char* ip_address = (recval_locs[1]==-1 ? "127.0.0.1" : argv[recval_locs[1]+1]);
	int port_num = (recval_locs[2]==-1 ? 3000 : atoi(argv[recval_locs[2]+1]));
	if(recval_locs[4]==-1){
		printf("DEBUG: No account name given to atm.\n");
		exit(255);
	}
	char* account = argv[recval_locs[4]+1];
	char* card_file_name = (recval_locs[3]==-1 ? "<account>.card" : argv[recval_locs[3]+1]);
	if(recval_locs[5]==-1){
		printf("DEBUG: No mode of operation given to atm.\n");
		exit(255);
	}
	char* operation_value = (mode_of_operation==3 ? "0.00" : (argv[recval_locs[5]+1]));
	

	printf("DEBUG: Auth file name is %s\n",auth_file_name);
	printf("DEBUG: IP address is %s\n",ip_address);
	printf("DEBUG: Port number is %d\n",port_num);
	printf("DEBUG: Account name is %s\n",account);
	printf("DEBUG: Card file name is %s\n",card_file_name);
	if(mode_of_operation!=3){
		printf("DEBUG: Mode of operation corresponds to %d and value is %s\n",mode_of_operation,operation_value);
	}else{
		printf("DEBUG: Mode of operation is -g\n");
	}

	exit(255);



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
