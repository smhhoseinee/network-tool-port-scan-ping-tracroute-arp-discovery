#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


/*
 *
 * ask user 3 for options 
 *
 *
 *
 * */

const int MAX_IP_STR_LEN = 17;
const int MAX_PORT_STR_LEN = 7;
const int MAX_MSG_OUT_LEN = 128;

void remove_cr(char *str){
	for(int i=0;i<strlen(str); i++){
		if(str[i] == '\n'){
			str[i] = '\0';
			return;
		}
	}
}

int input_port(char *port_str){
	puts("SERVER PORT: [unit 16] ");
	fgets(port_str,MAX_PORT_STR_LEN,stdin);
	remove_cr(port_str);
	return 0;
}

int ask_what_to_do(char *port_str){

	int choice;
	puts("select what you want:\n");
	puts("1-scan all ports\n");
	puts("2-just for common ports\n");
	puts("3-request for specific port \n");


	choice = getchar();
	getc(stdin);

	choice -= '0';


	switch(choice)
	{
		case 1:
			printf("you chose 1\n");
			break;

		case 2:
			printf("you chose 2\n");
			break;

		case 3:
			printf("you chose 3\n");
			input_port(port_str);
			break;

			// operator doesn't match any case constant +, -, *, /
		default:
			printf("Error! operator is not correct");
	}
	return 0;
}


int input_ip(char *server_addr_str ){
	puts("SERVER IP: [x.x.x.x (ipv4)] ");
	fgets(server_addr_str,MAX_IP_STR_LEN,stdin);
	remove_cr(server_addr_str);
	return 0;
}

int ask_ip_or_host(char *server_addr_str){

	 int choice;
         puts("choose your input method:\n");
         puts("1-ip\n");
         puts("2-hostname(domain)\n");

	 choice = getchar();
	 getc(stdin);

         puts("choice done\n");
	 choice -= '0';

         puts("2-hostname(domain)\n");

	switch(choice)
	{
		case 1:
			printf("you chose ip\n");
			input_ip(server_addr_str );
			printf("2you chose ip\n");
			break;

		case 2:
			printf("you chose hostname\n");
			//input host function
			break;

			// operator doesn't match any case constant +, -, *, /
		default:
			printf("Error! operator is not correct");
	}


       	return 0;
}

int main(){


	// get ip and port
	char *server_addr_str = malloc(sizeof(char) * MAX_IP_STR_LEN);
	char *port_str = malloc(sizeof(char) * MAX_PORT_STR_LEN);

	if(server_addr_str == NULL){
		fputs("memory allocation failed (ip)",stderr);
		exit(EXIT_FAILURE);
	}
	if(port_str == NULL){
		fputs("memory allocation failed (port)",stderr);
		exit(EXIT_FAILURE);
	}
	memset(server_addr_str, '\0' , MAX_IP_STR_LEN);
        memset(port_str, '\0', MAX_PORT_STR_LEN);

//	ask_ip_or_host(server_addr_str,port_str);
	ask_ip_or_host(server_addr_str);

	ask_what_to_do(port_str);

	puts(server_addr_str);
        puts(port_str);

	//convert port to useful format
	in_port_t server_port = atoi(port_str);




	//create struct sockaddr
	struct sockaddr_in server_address;
	memset(&server_address,0,sizeof(server_address));
	server_address.sin_family = AF_INET;
        int numerical_address = inet_pton(AF_INET,server_addr_str, &server_address.sin_addr.s_addr);

	if(numerical_address == 0){
                fputs("invalid IP", stderr);
                exit(EXIT_FAILURE);
        }
        if(numerical_address < 0){
                fputs("p to n failed",stderr);
                exit(EXIT_FAILURE);
        }
        server_address.sin_port = htons(server_port);




	//socket() creation
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP  );
	if(sock < 0){
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}
        if(connect(sock, (const struct sockaddr *)&server_address, sizeof(server_address))<0){
                perror("connection failed!");
                exit(EXIT_FAILURE);
        }else{
		printf("port %s is open",port_str);
	}

	return 0; 
}
