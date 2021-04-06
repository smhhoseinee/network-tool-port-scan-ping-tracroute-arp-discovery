
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080

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
	
	int sock = 0, valread;



	struct sockaddr_in serv_addr;
	char *msg = "Hi I'm smh";
	char buffer[1024]={0};

	sock = socket(AF_INET,SOCK_STREAM,0);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	

	send(sock,msg,strlen(msg),0);
	puts("msg sent");
	

	valread = read(sock,buffer,1024);
	printf("ans received:");
	printf("%s\n",buffer);




	return 0;
}
