#include "atm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/err.h>

// Default port and ip address are defined here

int sym_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext){
	EVP_CIPHER_CTX *ctx;
	int len;
	int ciphertext_len;

	// creat and init context
	if(!(ctx = EVP_CIPHER_CTX_new())){
		printf("DEBUG: Wuhwoh things are getting fonky at line 16\n");
		exit(255);
	}

	// init encryption operation 
	if(1!= EVP_EncryptInit_ex(ctx,EVP_aes_256_cbc(),NULL,key,iv)){
		printf("DEBUG: Wuhwoh things are getting fonky at line 22\n");
		exit(255);
	}

	// provide message to be encrypted/ get output
	if(1!= EVP_EncryptUpdate(ctx,ciphertext,&len,plaintext,plaintext_len)){
		printf("DEBUG: Wuhwoh things are getting fonky at line 28\n");
		exit(255);
	}
	ciphertext_len = len;

	// finalize the encryption 
	if(1!=EVP_EncryptFinal_ex(ctx,ciphertext+len,&len)){
		printf("DEBUG: Wuhwoh things are getting fonky at line 28\n");
		exit(255);
	}
	ciphertext_len += len;

	// clean up and return 
	EVP_CIPHER_CTX_free(ctx);
	return ciphertext_len;
}

int main(int argc, char** argv){
  	unsigned short port = 3000;
	char *ipAddr = "127.0.0.1";
	FILE *file;
	

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
	char account[123];
	memset(account,'\0',sizeof(account));
	strncpy(account,argv[recval_locs[4]+1],122);
	printf("DEBUG: account name is %d characters long, need to pad %d characters of space to the end.\n",strlen(account),122-strlen(account));
	char natural_card_buff[128]; // holds the value of the natural card associated with this account if needed later
	strcpy(natural_card_buff,account);
	strcat(natural_card_buff,".card");
	for(int i=strlen(account);i<122;i++){
		strcat(account," ");
	}
	

	char* card_file_name = (recval_locs[3]==-1 ? natural_card_buff : argv[recval_locs[3]+1]);
	if(recval_locs[5]==-1){
		printf("DEBUG: No mode of operation given to atm.\n");
		exit(255);
	}
	char operation_value[14];
	memset(operation_value,'\0',sizeof(operation_value));
	strncpy(operation_value,(mode_of_operation==3 ? "0.00" : (argv[recval_locs[5]+1])),13);
	printf("DEBUG: operation_value is %d characters long, need to pad %d characters of space to the end.\n",strlen(operation_value),13-strlen(operation_value));
	for(int i=strlen(operation_value);i<13;i++){
		strcat(operation_value," ");
	}
	

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

	// Now validate the values
	// Auth file name valid? 
	regex_t re;
	int response = 0;
	// regcomp(&re,"[_\\-\\.0123456789a-z]+",0);
	// response = regexec(&re,auth_file_name,0,NULL,0);
	// regfree(&re);
	if(response!=0||strcmp("..",auth_file_name)==0||strcmp(".",auth_file_name)==0){
		printf("DEBUG: Auth file does not fit the proper file name format.\n");
		exit(255);
	}

	if(!(file = fopen(auth_file_name,"r"))){
		printf("DEBUG: Auth file does not exist. Exiting with error.\n");
		exit(255);
	}

	// Mode of operation to char 
	char mode_char[3];
	if(mode_of_operation==0){
		strcpy(mode_char,"n");
	}else if (mode_of_operation==1){
		strcpy(mode_char,"d");
	}else if(mode_of_operation==2){
		strcpy(mode_char,"w");
	}else if(mode_of_operation==3){
		strcpy(mode_char,"g");
	}else{
		printf("DEBUG: Mode of operation doesn't match accepted values. This code should never be reached.\n");
		exit(255);
	}


	ATM *atm = atm_create(ip_address, port_num);

	/* get authenticated */
	// get card file set up 
	// unsigned char card_file_buffer[32] = "card5678911234567892123456789312";
	unsigned char card_file_buffer[32];
	FILE *card_file;
	if(mode_of_operation==0){
		// if making new, write a random 32 bytes to a file 
		if((card_file = fopen(card_file_name,"r"))){
			fclose(card_file);
			printf("DEBUG: Card file already exists. Exiting with error.\n");
			// TODO this breaks it for some reason
			exit(255);
		}else{
			if((card_file = fopen(card_file_name,"w"))){
				printf("DEBUG: Writing random bytes to card file.\n");
				RAND_bytes(card_file_buffer,32);
				fwrite(card_file_buffer,1,sizeof(card_file_buffer),card_file);
				fclose(card_file);			
			}
		}		
	}else{
		// if using existing, read from that file specified
		if((card_file = fopen(card_file_name,"r"))){
			printf("DEBUG: Reading from existing card file at %s.\n",card_file_name);
			fread(card_file_buffer, sizeof(card_file_buffer),1,card_file);
			fclose(card_file);
		}else{
			printf("DEBUG: Card file does not exist. Exiting with error.\n");
			exit(255);
			// TODO this breaks bank for some reason
		}
	}

	// get auth file set up 
	unsigned char auth_file_buffer[32];
	FILE *auth_file;
	if((auth_file = fopen(auth_file_name,"r"))){
		printf("DEBUG: Reading from auth file at %s.\n",auth_file_name);
		fread(auth_file_buffer, sizeof(auth_file_buffer),1,auth_file);
		fclose(auth_file);
	}else{
		printf("DEBUG: Auth file does not exist. Exiting with error.\n");
		exit(255);
	}
	// TODO 

	/* send messages */

	/* Buffer message format:
		0 account name (122 characters)
		1 card file value (32 characters)
		2 mode of operation (1 character)
		3 value of operation (13 characters)
		4 anti-repeat attack value (32 characters)
	*/
	char buffer[300] = "";
	int buffer_idx = 0;
	// strcat(buffer,account); // always 122 characters
	for(int i=0;i<122;i++){
		buffer[buffer_idx++] = account[i];
	}
	// strcat(buffer,card_file_buffer); // always 32 wacky characters
	for(int i=0;i<32;i++){
		buffer[buffer_idx++] = card_file_buffer[i];
	}
	// strcat(buffer,mode_char);
	for(int i=0;i<strlen(mode_char);i++){
		buffer[buffer_idx++] = mode_char[i];
	}

	printf("Operation value is equal to '%s'.\nIt is %d characters long.\n",operation_value,strlen(operation_value));
	strcat(buffer,operation_value);
	for(int i=0;i<strlen(operation_value);i++){
		buffer[buffer_idx++] = operation_value[i];
	}

	// encrypt here 
	// unsigned char *ciphertext = malloc(300*sizeof(char*)+16);
	// unsigned char iv[16];
	// RAND_bytes(iv,16);
	// size_t iv_len = 16;

	// int ciphertext_len = sym_encrypt(buffer, strlen((char*)buffer),auth_file_buffer,iv,ciphertext);

	// for(int i=ciphertext_len;i<ciphertext_len+16;i++){
	// 	ciphertext[i] = iv[i-ciphertext_len];
	// }

	// atm_send(atm, ciphertext, ciphertext_len+16);

	atm_send(atm, buffer, sizeof(buffer));
	atm_recv(atm, buffer, sizeof(buffer));


	// decrypt here 


	printf("atm received %s\n", buffer);
	
	atm_free(atm);
	

	// Implement how atm protocol will work: sanitizing inputs and using different modes of operations

	return EXIT_SUCCESS;
}
