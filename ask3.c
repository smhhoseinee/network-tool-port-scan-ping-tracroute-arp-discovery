#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


/*
 * ask host or ip
 * input host or ip
 * convert host to ip
 * ask user 3 options 
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

in_port_t input_port(char *port_str){
	puts("SERVER PORT: [unit 16] ");
	fgets(port_str,MAX_PORT_STR_LEN,stdin);
	remove_cr(port_str);

	in_port_t server_port = atoi(port_str);

	return server_port;
}

int input_domain(char *server_addr_str ){



	return 0;
}

int input_ip(char *server_addr_str ){
	puts("SERVER IP: [x.x.x.x (ipv4)] ");
	fgets(server_addr_str,MAX_IP_STR_LEN,stdin);
	remove_cr(server_addr_str);
	return 0;
}

int create_socket(){
	//socket() creation
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP  );
	if(sock < 0){
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	return sock;
}

int connect_socket(int sock ,struct  sockaddr_in server_address){
        if(connect(sock, (const struct sockaddr *)&server_address, sizeof(server_address))<0){
		printf("port %d is not open", ntohs(server_address.sin_port));
                perror("connection failed!");
                exit(EXIT_FAILURE);
        }else{
		printf("port %d is open", ntohs(server_address.sin_port));
	}
	return 0;
}


struct sockaddr_in create_struct_sockaddr(struct sockaddr_in server_address, char *server_addr_str, in_port_t server_port){
	//create struct sockaddr
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

	return server_address;


}

int input_port_range(){


	return 0;
}

int scan_port(char *server_addr_str,in_port_t server_port){
	struct sockaddr_in server_address;
	server_address = create_struct_sockaddr(server_address,server_addr_str,server_port);

	int sock = create_socket();
	connect_socket(sock,server_address);


	return 0;
}

int input_and_scan_port(char *server_addr_str,char *port_str){
	in_port_t server_port = input_port(port_str);
	scan_port(server_addr_str,server_port);
	return 0;
}

const int NUMBER_OF_SERVICES = 10;
int ask_port_service(char *server_addr_str,char *port_str){
	int choice;
	char*  port_of_service[NUMBER_OF_SERVICES];
	for(int i=0;i<NUMBER_OF_SERVICES;i++){
		port_of_service[i] = malloc(sizeof(char) * MAX_PORT_STR_LEN);
	}

	int i = 1;
	port_of_service[i]="80";
	i++;
	port_of_service[i]="443";
	i++;
	port_of_service[i]="25";
	i++;
	port_of_service[i]="21";
	i++;
	port_of_service[i]="23";
	i++;
	port_of_service[i]="22";
	i++;
	

	puts("select service you want to scan:\n");
	puts("1-HTTP(80)\n");
	puts("2-TLS(443)\n");
	puts("3-SMTP(25)\n");
	puts("4-FTP(21)\n");
	puts("5-TELNET(23)\n");
	puts("6-SSH(22)\n");
	

	choice = getchar();
	getc(stdin);
	choice -= '0';


	printf("you chose service %d \n",choice);

	port_str = port_of_service[choice];
	in_port_t server_port = atoi(port_str);
	scan_port(server_addr_str,server_port);

	return 0;


}

int ask_what_to_do(char *server_addr_str, char *port_str){

	int choice;
	puts("select what you want:\n");
	puts("1-scan all ports\n");
	puts("2-just for common ports\n");
	puts("3-request for specific port \n");
	puts("4-request for specific services\n");
	puts("4-request for specific range\n");

	choice = getchar();
	getc(stdin);

	choice -= '0';

	printf("you chose %d \n",choice);
	switch(choice)
	{
		case 1:
			break;

		case 2:
			break;

		case 3:

			input_and_scan_port(server_addr_str,port_str);
			break;
		case 4:
			ask_port_service(server_addr_str,port_str);
			break;
		case 5:

			break;


			// operator doesn't match any case constant +, -, *, /
		default:
			printf("Error! operator is not correct");
	}
	return 0;
}


int ask_ip_or_host(char *server_addr_str, char *port_str){

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

	ask_what_to_do(server_addr_str, port_str);

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
	ask_ip_or_host(server_addr_str, port_str);


	//convert port to useful format
  //	in_port_t server_port = atoi(port_str);




	//struct sockaddr_in server_address;
	//server_address = create_struct_sockaddr(server_address,server_addr_str);


	return 0; 
}
