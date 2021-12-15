#include "bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
		// printf("DEBUG: Wuhwoh things are getting fonky at line 16\n");
		exit(255);
	}

	// init encryption operation 
	if(1!= EVP_EncryptInit_ex(ctx,EVP_aes_256_cbc(),NULL,key,iv)){
		// printf("DEBUG: Wuhwoh things are getting fonky at line 22\n");
		exit(255);
	}

	// provide message to be encrypted/ get output
	if(1!= EVP_EncryptUpdate(ctx,ciphertext,&len,plaintext,plaintext_len)){
		// printf("DEBUG: Wuhwoh things are getting fonky at line 28\n");
		exit(255);
	}
	ciphertext_len = len;

	// finalize the encryption 
	if(1!=EVP_EncryptFinal_ex(ctx,ciphertext+len,&len)){
		// printf("DEBUG: Wuhwoh things are getting fonky at line 28\n");
		exit(255);
	}
	ciphertext_len += len;

	// clean up and return 
	EVP_CIPHER_CTX_free(ctx);
	return ciphertext_len;
}

int sym_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext){
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;

	// create and init context
	if(!(ctx = EVP_CIPHER_CTX_new())){
		// printf("DEBUG: Wuhwoh things are getting fonky at line 20\n");
		exit(255);
	}

	// init decrypt operation
	if(1!= EVP_DecryptInit_ex(ctx,EVP_aes_256_cbc(),NULL,key,iv)){
		// printf("DEBUG: Wuhwoh things are getting fonky at line 26\n");
		exit(255);
	}

	// put decrypted message, get output
	if(1!= EVP_DecryptUpdate(ctx,plaintext,&len,ciphertext,ciphertext_len)){
		// printf("DEBUG: Wuhwoh things are getting fonky at line 32\n");
		exit(255);
	}
	plaintext_len = len;

	//Cleanup 
	EVP_CIPHER_CTX_free(ctx);
	return plaintext_len;
}
#define ull unsigned long long
struct linked_list_node {
	ull account_balance;
	char *account; // key
	// char *balance_str;
	unsigned char *card_info;
	struct linked_list_node *next;
};

struct linked_list_node *head = NULL;
struct linked_list_node *curr = NULL;

// Converts ull to string
char * utoa(unsigned int n)
{
  char * res, buf[30]; // long enough for largest number
  unsigned int i, counter = 0;

  if (n == 0)
    buf[counter++] = '0';

  for ( ; n; n /= 10)
    buf[counter++] = "0123456789"[n%10];

  res = malloc(counter);

  for (i = 0; i < counter; ++i)
    res[i] = buf[counter - i - 1];
  res[i] = '\0';
  return res;
}

void insert(char *account,char *account_balance,unsigned char *card_info){
	int neg_check = atoi(account_balance);
	printf("DEBUG: Received initial bal %s\n", account_balance);
	if (neg_check < 0) {
		printf("ERROR: Please deposit a positive number\n");
		exit(255);
	}
	ull bal = (ull) atoi(account_balance);
	if (bal >= 4294967295.99) {
		printf("ERROR: Value to deposit too big -- please deposit in smaller increments\n");
		return;
	}
	struct linked_list_node *new_node = (struct linked_list_node*)malloc(sizeof(struct linked_list_node));
	
	new_node->account = malloc(strlen(account)+1);
	// memset(new_node->account,'\0',123);
	strcpy(new_node->account,account);
	// strcpy(new_node->balance_str, account_balance);

	new_node->account_balance = (ull) 0;
	// memset(new_node->account_balance,'\0',15);
	new_node->account_balance += bal;

	new_node->card_info = malloc(strlen(card_info)+1);
	// memset(new_node->card_info,'\0',33);
	strcpy(new_node->card_info,card_info);

	new_node->next = head;
	head = new_node;
}

void printLinkedList(){
	struct linked_list_node *curr = head;
	printf("\n[ ");

	while(curr != NULL){
		printf("(account:%s,\n  account_balance:%u,\n  card_info:%s)\n",curr->account,curr->account_balance ,curr->card_info);
		curr = curr->next;
	}

	printf(" ]\n");
}

struct linked_list_node* find_account(char  *account){
	struct linked_list_node* curr = head;
	if(head==NULL){
		return NULL;
	}
	while(strcmp(curr->account,account)!=0){
		if(curr->next == NULL){
			return NULL;
		}else{
			curr = curr->next;
		}
	}
	return curr;
}

int main(int argc, char** argv){
	int i = 1;
	char *ip = "127.0.0.1";
	FILE *file;
	int auth_pos = -1; // returns the location in argv of an auth file. If not specified, it is -1.

	int recval_locs[] = {-1,-1};
	while(i<argc){
		if(strcmp(argv[i],"-p")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(recval_locs[0]!=-1){
				// printf("DEBUG: Duplicate %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				// printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				recval_locs[0] = i;
			}
			i+=2;
		}else if(strcmp(argv[i],"-s")==0){
			// printf("DEBUG: %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
			if(recval_locs[1]!=-1){
				// printf("DEBUG: Duplicate %s argument detected at %d with %s value\n",argv[i],i,argv[i+1]);
				exit(255);
			}else if(i+1>=argc){
				// printf("DEBUG: Argument %s detected at %d has no value\n",argv[i],i);
				exit(255);
			}else{
				recval_locs[1] = i;
			}
			i+=2;
		}else if(strlen(argv[i])>2&&argv[i][0]=='-'&&argv[i][1]=='p'){
			// printf("DEBUG: %c argument at %d was a nospace attempt.\n",argv[i][1],i);
			if(recval_locs[0]!=-1){
				// printf("DEBUG: Duplicate %c argument detected at %d with %s value.\n",argv[i][1],i,"TODO");
				exit(255);
			}else{
				recval_locs[0] = -1-i;
			}
			i+=1;
		}else if(strlen(argv[i])>2&&argv[i][0]=='-'&&argv[i][1]=='s'){
			// printf("DEBUG: %c argument at %d was a nospace attempt.\n",argv[i][1],i);
			if(recval_locs[1]!=-1){
				// printf("DEBUG: Duplicate %c argument detected at %d with %s value.\n",argv[i][1],i,"TODO");
				exit(255);
			}else{
				recval_locs[1] = -1-i;
			}
			i+=1;
		}else{
			// printf("DEBUG: Argument at %d was invalid. Value is %s\n",i,argv[i]);
			exit(255);
		}
	}
	
	int port_num;
	unsigned short port;
	if(recval_locs[0]<-1){
		// printf("Preparing to look for port at %d\n",(recval_locs[0]+1)*-1);
		port_num = atoi(&(argv[(recval_locs[0]+1)*-1][2]));
	}else{
		port_num = (recval_locs[0]==-1 ? 3000 : atoi(argv[recval_locs[0]+1]));
	} 
	port = (unsigned short)port_num;

	char *auth_file_name;
	if(recval_locs[1]<-1){
		// printf("Preparing to look for authfile at %d\n",(recval_locs[1]+1)*-1);
		auth_file_name = &(argv[(recval_locs[1]+1)*-1][2]);
	}else{
		auth_file_name = (recval_locs[1]==-1 ? "bank.auth" : argv[recval_locs[1]+1]);
	}
	
	unsigned char sym_key[32];

	// print the values that we learned 
	// printf("DEBUG: The port, as entered, is %d\n",(int)port);
	// printf("DEBUG: The auth file is %s\n",auth_file_name);

	/* no error checking is done on any of this. may need to modify this */
	

	// if not auth_file exists, create auth_file
	//bank->auth_file = auth_file
	
	FILE *auth_file;
	if((auth_file = fopen(auth_file_name,"r"))){
		fclose(auth_file);
		// printf("DEBUG: auth file already exists. Exiting with error.\n");
		exit(255);
	}else{
		if((auth_file = fopen(auth_file_name,"w"))){
			// printf("DEBUG: writing super secret code to new auth file.\n");
			RAND_bytes(sym_key,32);
			fwrite(sym_key,1,sizeof(sym_key),auth_file);
			fclose(auth_file);
		}
	}

	Bank *b = bank_create(auth_file_name, ip, port);

	

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

		
		// decrypt here

		// first 16 bytes are the unsigned char iv
		unsigned char iv[16];
		unsigned char encrypted_msg[300];
		unsigned char decrypted_msg[300];
		// printf("DEBUG: Received message containing:\n(");
		for(int i=0;i<16;i++){
			//printf("%c",buffer[i]);
			iv[i] = buffer[i];
		}
		// printf(" iv\n");
		for(int i=16;i<192;i++){
			//printf("%c",buffer[i]);
			encrypted_msg[i-16] = buffer[i];
		}
		// printf(" msg)\n");
		int decrypted_length = sym_decrypt(encrypted_msg,192-16,sym_key,iv,decrypted_msg);
		// printf("DEBUG: length of decrypted message is %d.\nMessage is:(\n",decrypted_length);
		for(int i=0;i<decrypted_length+9;i++){
			//printf("%c",decrypted_msg[i]);
		}
		// printf(")\n");
		
		/* Buffer message format:
		0 account name (122 characters)
		1 card file value (32 characters)
		2 mode of operation (1 character)
		3 value of operation (13 characters)
		*/
		int buffer_idx = 0;
		char sent_account[123];
		// memset(sent_account,'\0',sizeof(sent_account));
		// strncpy(sent_account,buffer,122);
		for(int i=0;i<122;i++){
			sent_account[i] = decrypted_msg[buffer_idx++];
		}
		sent_account[122] = '\0';
		// printf("DEBUG: ATM sent an account with account name of %s.\n",sent_account);
		// printf("DEBUG: Remaining string is %s.\n",&(buffer[122]));

		unsigned char sent_card_value[33];
		// memset(sent_card_value,'\0',sizeof(sent_card_value));
		// strncpy(sent_card_value,&(buffer[122]),32);
		// printf("DEBUG: ATM sent a card value of: '");
		for(int i=0;i<32;i++){
			sent_card_value[i] = decrypted_msg[buffer_idx++];
			// printf("%c",sent_card_value[i]);
		}
		sent_card_value[32] = '\0';
		// printf("'.\n");
		// printf("DEBUG: ATM sent a card value of %s.\n",sent_card_value);
		// printf("DEBUG: Remaining string is %s.\n",&(buffer[122+32]));

		char sent_mode_of_operation[2];
		// memset(sent_mode_of_operation,'\0',sizeof(sent_mode_of_operation));
		// strncpy(sent_mode_of_operation,&(buffer[122+32]),2);
		for(int i=0;i<1;i++){
			sent_mode_of_operation[i] = decrypted_msg[buffer_idx++];
		}
		sent_mode_of_operation[2] = '\0';
		// printf("DEBUG: ATM sent a mode of operation of %s.\n",sent_mode_of_operation);
		// printf("DEBUG: Remaining string is %s.\n",&(buffer[122+32+2]));

		char sent_value_of_operation[15];
		// memset(sent_value_of_operation,'\0',sizeof(sent_value_of_operation));
		// strncpy(sent_value_of_operation,&(buffer[122+32+2]),13);
		for(int i=0;i<13;i++){
			sent_value_of_operation[i] = decrypted_msg[buffer_idx++];
		}
		sent_value_of_operation[13] = '\0';
		char *p = strchr(sent_value_of_operation, '.');
		// THIS GETS THE AMOUNT OF MONEY AS AN EXPRESSION OF CENTS -- WILL FINISH LATER
		*(p) = *(p + 1);
		*(p + 1) = *(p + 2);
		*(p + 2) = '\0';
		printf("DEBUG: ATM sent a value of operation of %s.\n",sent_value_of_operation);
		// printf("DEBUG: Remaining string (corresponding to the anti_repeat value) is %s.\n",&(buffer[122+32+1+13]));

		
		// do some validation ?
		// TODO validation

		// if data validates: 

		// TODO: Check if the card for the user validates 
		// TODO: Implement a system to check 
		int is_valid = 1;
		struct linked_list_node *acc;
		char final_value[300] = "\"account\":\"";
		strcat(final_value, sent_account);
		if('n'==sent_mode_of_operation[0]){
			// printf("DEBUG: The atm wants to make a new account.\n");
			if(find_account(sent_account)!=NULL){
				// printf("DEBUG: That account for '%s' already exists!\n",sent_account);
				is_valid = 0;
			}else{
				insert(sent_account,sent_value_of_operation,sent_card_value);
				ull val = (ull) atoi(sent_value_of_operation);
				printf("DEBUG -- VAL = %s\n", utoa(val / 100));
				strcat(final_value, "\", \"initial_balance\":");
				strcat(final_value, utoa(val / 100));
				strcat(final_value, ".");
				if (val % 100 != 0) {
					strcat(final_value, utoa(val % (ull) 100));
				} else {
					strcat(final_value, "00");
				}
				//printLinkedList();
			}
		}else if('d'==sent_mode_of_operation[0]){
			// printf("DEBUG: The atm wants to deposit.\n");
			struct linked_list_node *found = find_account(sent_account);
			if(found==NULL){
				// printf("DEBUG: The account for '%s' does not exist!\n",sent_account);
				is_valid = 0;
			}else{
				// check if card is valid
				int valid = 1;
				for(int i=0;i<32;i++){
					if(found->card_info[i]!=sent_card_value[i]){
						valid = 0;
					}
				}
				if(valid == 1){
					acc = found;
					// printf("DEBUG: Changing found's values..... matthew started this hopefully its finished and/or working.....\n");
					// TODO move this operation to later until the atm has passed the repeat attack check
					ull val = (ull) atoi(sent_value_of_operation);
					if ((val + found->account_balance) < LLONG_MAX) {
						found->account_balance += val;
						strcat(final_value, "\", \"deposit\":");
						strcat(final_value, utoa(val / (ull) 100));
						strcat(final_value, ".");
						if (val % 100 != 0) {
							strcat(final_value, utoa(val % (ull) 100));
						} else {
							strcat(final_value, "00");
						}
						//printLinkedList();
					} else {
						printf("ERROR -- Max account limit exceeded -- please withdraw some funds\n");
						is_valid = 0;
					}
				}else{
					// printf("DEBUG: Woah woah buddy, you dont have the right card to access that account.\n");
					is_valid = 0;
				}
			}
		}else if('w'==sent_mode_of_operation[0]){
			// printf("DEBUG: The atm wants to widthdraw.\n");
			struct linked_list_node *found = find_account(sent_account);
			if(found==NULL){
				// printf("DEBUG: The account for '%s' does not exist!\n",sent_account);
				is_valid = 0;
			}else{
				// check if card is valid
				int valid = 1;
				for(int i=0;i<32;i++){
					if(found->card_info[i]!=sent_card_value[i]){
						valid = 0;
					}
				}
				if(valid == 1){
					acc = found;
					// printf("DEBUG: Changing found's values..... implement this as soon as I know whether we have to store superlarge numbers or not.....\n");
					ull val = (ull) atoi(sent_value_of_operation);
					if ((int) (found->account_balance - val) >= 0) {
						found->account_balance -= val;
						strcat(final_value, "\", \"withdrawal\":");
						strcat(final_value, utoa(val / (ull) 100));
						strcat(final_value, ".");
						if (val % 100 != 0) {
							strcat(final_value, utoa(val % (ull) 100));
						} else {
							strcat(final_value, "00");
						}
					} else {
						printf("DEBUG -- Tried to withdraw more than you had D:\n");
						is_valid = 0;
						// TODO -- FIND A SOLUTION TO THIS USAGE ERROR
					}
					//printLinkedList();
				}else{
					// printf("DEBUG: Woah woah buddy, you dont have the right card to access that account.\n");
					is_valid = 0;
				}
			}
		}else if('g'==sent_mode_of_operation[0]){
			// printf("DEBUG: The atm wants to check the balance of an account.\n");
			struct linked_list_node *found = find_account(sent_account);
			if(found==NULL){
				// printf("DEBUG: The account for '%s' does not exist!\n",sent_account);
				is_valid = 0;
			}else{
				// check if card is valid
				int valid = 1;
				for(int i=0;i<32;i++){
					if(found->card_info[i]!=sent_card_value[i]){
						valid = 0;
					}
				}
				if(valid == 1){
					acc = found;
					// printf("DEBUG: Changing found's values..... implement this as soon as I know whether we have to store superlarge numbers or not.....\n");
					// printf("Account for %s has %u funds\n", found->account, found->account_balance);
					ull val = (ull) atoi(found->account_balance);
					strcat(final_value, "\", \"balance:\":");
					strcat(final_value, utoa(val / (ull) 100));
					strcat(final_value, ".");
					if (val % 100 != 0) {
						strcat(final_value, utoa(val % (ull) 100));
					} else {
						strcat(final_value, "00");
					}
					//printLinkedList();
				}else{
					// printf("DEBUG: Woah woah buddy, you dont have the right card to access that account.\n");
					is_valid = 0;
				}
			}
		}else{
			// printf("DEBUG: The atm sent an invalid mode of operation: '%c'.\n",sent_mode_of_operation[0]);
			is_valid = 0;
		}

		// msg asking for modified echo here
		if(is_valid==1){
			// printf("DEBUG: Communication is valid so far- defending against repeat attacks.\n");
			// do everything
			/* Buffer message format:
				0 account name (122 characters)
				1 card file value (32 characters)
				2 mode of operation (1 character)
				3 value of operation (13 characters)
			*/
			char buffer_resp[300] = "";
			// created from msg above: char sent_account[123];
			// to be filled in with random bytes: unsigned char card_rand_bytes[32];
			unsigned char card_rand_bytes[32];
			RAND_bytes(card_rand_bytes,32);

			// value doesnt matter
			char operation_value[14];
			memset(operation_value,'\0',sizeof(operation_value));
			for(int i=strlen(operation_value);i<13;i++){
				strcat(operation_value," ");
			}

			int buffer_idx_resp = 0;
			// always 122 characters
			for(int i=0;i<122;i++){
				buffer_resp[buffer_idx_resp++] = sent_account[i];
			}
			// always 32 wacky characters
			for(int i=0;i<32;i++){
				buffer_resp[buffer_idx_resp++] = card_rand_bytes[i];
			}
			// placeholder value
			// for(int i=0;i<strlen("X");i++){
			// 	buffer_resp[buffer_idx_resp++] = 'X';
			// }
			// 1 char

			// printf("Operation value is equal to '%s'.\nIt is %d characters long.\n",operation_value,strlen(operation_value));
			// strcat(buffer_resp,operation_value);
			// for(int i=0;i<strlen(operation_value);i++){
			// 	buffer_resp[buffer_idx_resp++] = operation_value[i];
			// }
			// 13 chars

			// printf("DEBUG: Printing entire message:(\n");
			// for(int i=0;i<buffer_idx_resp;i++){
			// 	printf("%c",buffer_resp[i]);
			// }
			// printf(")\n");

			// encrypt here 
			unsigned char *ciphertext = malloc(300*sizeof(char*));
			// unsigned char iv[16]; is read in above
			// unsigned char sym_key[32]; is read in above

			int ciphertext_len = sym_encrypt(buffer_resp, strlen((char*)buffer_resp),sym_key,iv,ciphertext);

			unsigned char *msg_resp = malloc(300*sizeof(char*)+16);
			for(int i=0;i<16;i++){
				msg_resp[i] = iv[i];
			}
			for(int i=16;i<16+ciphertext_len;i++){
				msg_resp[i] = ciphertext[i-16];
			}
			int msg_resp_len = ciphertext_len+16;

			// printf("DEBUG: Preparing to send message size %d containing:\n(",msg_resp_len);
			// for(int i=0;i<16;i++){
			// 	printf("%c",msg_resp[i]);
			// }
			// printf(" iv\n");
			// for(int i=16;i<msg_resp_len;i++){
			// 	printf("%c",msg_resp[i]);
			// }
			// printf(" msg)\n");
			bank_send(b, msg_resp, msg_resp_len);

			char buffer_rec2[1024];
			bank_recv(b, buffer_rec2, sizeof(buffer_rec2));
			// for(int i=0;i<208;i++){
			// 	printf("%c",buffer_rec2[i]);
			// }
			// printf(" msg");
			unsigned char decrypted_msg_rec2[300];
			int decrypted_length_rec2 = sym_decrypt(buffer_rec2,208,sym_key,iv,decrypted_msg_rec2);
			// printf("DEBUG: Received decrypted message of size %d containing:(\n",decrypted_length_rec2);
			// for(int i=0;i<122;i++){
			// 	printf("%c",decrypted_msg_rec2[i]);
			// }
			// printf(" acct\n");
			for(int i=122;i<122+16;i++){
				//printf("%c",decrypted_msg_rec2[i]);
				if(card_rand_bytes[i-122]!=decrypted_msg_rec2[i]){
					// printf("(%c!=%c)",card_rand_bytes[i-122],decrypted_msg_rec2[i]);
					// TODO close connection
					// printf("DEBUG: Repeat attack detected! Terminating the connection.\n");
					// TODO terminate the connection
				}
			}
			// printf(" val\n");
			// printf("DEBUG: Expected :(\n");
			// for(int i=0;i<16;i++){
			// 	printf("%c",card_rand_bytes[i]);
			// }
			// printf(" value\n");

			// Verified that this is not a repeat attack
			printf("DEBUG: Everything so far is valid- changes to state are now going to be made and returned.\n");
			

			// MATTHEW TODO MAKE CHANGES TO BANK STATE HERE- EVERYHTING IS VERIFIED NOW
			// Am going to finish this tomorrow -- want to parse this string to be accurate to last transaction
			// Shouldn't be too tough
			//char final_value[300] = "\"account\":\"bob\",\"initial_balance\":\"10.00\"\0";
			// char final_value[300] = acc->account;
			// char bal[20];
			// strcat(final_value, "\"balance\":");
			// sprintf(bal, "%u", acc->account_balance);
			// strcat(final_value, bal);
			// strcat(final_value, "\"\0");

			printLinkedList();
			// CHANGES ARE DONE BEING MADE HERE
			// NOW PRINTING FINAL VALUE FOR BANK
			for(int i=strlen(final_value);i<300;i++){
				strcat(final_value," ");
			}
			for(int i=0;i<300;i++){
				if(final_value[i]!=' '){
					printf("%c",final_value[i]);
				}
			}
			printf("\n");

			// always 300 characters
			unsigned char final_buffer_resp[300] = "";
			for(int i=0;i<300;i++){
				final_buffer_resp[i] = final_value[i];
			}

			unsigned char final_ciphertext[500];
			int final_ciphertext_len = sym_encrypt(final_buffer_resp, 300 ,sym_key,iv,final_ciphertext);

			unsigned char final_message[600];
			for(int i=0;i<16;i++){
				final_message[i] = iv[i];
			}
			for(int i=16;i<16+final_ciphertext_len;i++){
				final_message[i] = final_ciphertext[i-16];
			}
			int final_message_len = final_ciphertext_len+16;
			// decrypted message is 300 characters long 
			// printf("DEBUG: Final encrypted message is %d characters long.\n",final_message_len);
			bank_send(b, final_message, final_message_len);
		}else{
			printf("DEBUG: Operation was not valid- no state changes will be made.\n");
		}
		

		/* when finished processing commands ...*/
		close(b->clientfd);
		b->clientfd = -1;
	}


	
	
	// Implement how atm protocol will work: sanitizing inputs and using different modes of operations

	return EXIT_SUCCESS;
}
