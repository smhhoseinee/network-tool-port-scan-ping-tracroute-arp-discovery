
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


/*
 * ip from user
 * port from user
 *
 * validation
 *
 * connect to server
 *
 * loop =>
 * 	msg from user
 * 	send to server
 * 	wait for server
 *
 * */

/*
 * int a = 5;
 * b = a + 1;
 *
 * int a* = malloc(sizeof(int));
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


int main(){

	// 1.1.1.1
	// 12312

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

	puts("SERVER IP: [x.x.x.x (ipv4)] ");
	fgets(server_addr_str,MAX_IP_STR_LEN,stdin);
	puts("SERVER PORT: [unit 16] ");
	fgets(port_str,MAX_PORT_STR_LEN,stdin);

	puts(server_addr_str);
	puts(port_str);


	remove_cr(server_addr_str);
	remove_cr(port_str);

	in_port_t server_port = atoi(port_str);

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP  );

	if(sock < 0){
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}


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

	if(connect(sock, (const struct sockaddr *)&server_address, sizeof(server_address))<0){
		perror("connection failed!");
		exit(EXIT_FAILURE);
	}

	for(;;){
		char *outgoing_msg = malloc(sizeof(char)*MAX_MSG_OUT_LEN);
		if(outgoing_msg == NULL){
			fputs("memory allocation failed." , stderr);
			exit(EXIT_FAILURE);
		}
		memset(outgoing_msg,'\0',MAX_MSG_OUT_LEN);


		puts("ENTER YOUR MSG () max length = 128, rest will be discarded");
		scanf("%128s",outgoing_msg);

		size_t msg_len = strlen(outgoing_msg);
		ssize_t bytes =  send(sock, outgoing_msg, msg_len,0);
	
		if(bytes < 0){
			perror("send failed");
			exit(EXIT_FAILURE);
		}else if(bytes != msg_len){
			fputs("unexpected number of bytes sent.",stderr);
			exit(EXIT_FAILURE);
		}

		unsigned int total_rcv_bytes = 0;
		puts("RECEIVED FROM SERVER:");
		while(total_rcv_bytes < bytes){
			char buffer[128] = {'\0'};
			bytes = recv(sock,buffer,MAX_MSG_OUT_LEN - 1, 0);

			if(bytes<0){
				perror("recv failed");
				exit(EXIT_FAILURE);
			} else if (bytes == 0){
			  perror("server was terminated");
			  exit(EXIT_FAILURE);
			}

			total_rcv_bytes += bytes;
			buffer[bytes] = '\0';

			printf("%s",buffer);
		}
		puts("");

	}

	close(sock);
	return 0; 
}
