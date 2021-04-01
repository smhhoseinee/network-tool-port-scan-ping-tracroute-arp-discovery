
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


const int PORT = 9001;
int main(){

	int sock;
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock <0){
		puts("socket creation failed");
		return -1;
	}

	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
	if(connection < 0){
		printf("connection failed");
		return -1;
	}



	char buffer[256] = {0};
	for(;;){
		int recv_status = recv(sock,&buffer,sizeof(buffer),0);
		printf("server : %s\n",buffer);

	}

	
	close(sock);
	return 0;

}
