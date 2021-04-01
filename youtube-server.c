
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


const int PORT = 9001;
const int msg_size = 256;
int main(){

	printf("before start");

	char server_message[256] = "Hi Client" ;

	int sock = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port= htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(sock,(struct sockaddr*) &server_address, sizeof(server_address));

	for(;;){
		listen(sock,5);

		int client_socket;
		client_socket = accept(sock,NULL,NULL);

		send(client_socket,server_message,sizeof(server_message),0);

		puts("enter your msg\n");
		fgets(server_message,msg_size,stdin);

		puts(server_message);

		//fgets(server_message);
		if(server_message == "quit"){
			break;
		}
	}

	close(sock);
	return 0;

}
