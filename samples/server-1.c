
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * port from user
 * start server (socket creation & building)
 *
 * loop => start listening & echoing
 * */


const int MAX_IP_STR_LEN = 17;
const int MAX_PORT_STR_LEN = 7;
const int MAX_MSG_OUT_LEN = 128;
const int MAX_PENDING = 5;

void remove_cr(char *str){
	for(int i=0;i<strlen(str); i++){
		if(str[i] == '\n'){
			str[i] = '\0';
			return;
		}
	}
}

void serve_client(int client_socket){
	char buffer[MAX_MSG_OUT_LEN];
	ssize_t bytes_rcv = recv(client_socket,buffer,MAX_MSG_OUT_LEN,0);

	if(bytes_rcv<0){
		perror("recv failed");
		exit(EXIT_FAILURE);
	}
	while(bytes_rcv > 0){
		ssize_t bytes_send = send(client_socket,buffer,bytes_rcv,0);

		if(bytes_send<0){
			perror("send failure");
			exit(EXIT_FAILURE);
		}else if(bytes_send != bytes_rcv){
			fputs("unexpected bytes sent",stderr);
			exit(EXIT_FAILURE);
		}

		bytes_rcv =  bytes_rcv = recv(client_socket,buffer,MAX_MSG_OUT_LEN,0);
		if(bytes_rcv<0){
			perror("recv failed");
			exit(EXIT_FAILURE);
		}
	}
	close(client_socket);

}

int main(){


	char *port_str = malloc(sizeof(char) * MAX_PORT_STR_LEN);

	if(port_str == NULL){
		fputs("memory allocation failed (port)",stderr);
		exit(EXIT_FAILURE);
	}
	
	memset(port_str, '\0', MAX_PORT_STR_LEN);

	puts("SERVER PORT: [unit 16] ");
	fgets(port_str,MAX_PORT_STR_LEN,stdin);

	remove_cr(port_str);

	in_port_t server_port = atoi(port_str);

	int sock_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP  );

	if(sock_server < 0){
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}


	struct sockaddr_in server_address;
	memset(&server_address,0,sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock_server, (const struct sockaddr *)&server_address, sizeof(server_address))<0){
		perror("bind failed!");
		exit(EXIT_FAILURE);
	}
	if(listen(sock_server, MAX_PENDING)<0){
		perror("listen failed");
		exit(EXIT_FAILURE);
	}



	for(;;){

		struct sockaddr_in client_address;
		socklen_t client_address_len = sizeof(client_address);

		int client_socket = accept(sock_server,(struct sockaddr *) &client_address,&client_address_len);

		if(client_socket<0){
			perror("accept failed");
			exit(EXIT_FAILURE);
		}


		char client_name[INET_ADDRSTRLEN];
		if(inet_ntop(AF_INET, &client_address.sin_addr.s_addr, client_name, sizeof(client_address))!=NULL){
			printf("serving [%s:%d]\n", client_name , ntohs(client_address.sin_port));
		}else{
		  puts("unable to get client name");
		}

		serve_client(client_socket);

	}

	return 0; 
}
