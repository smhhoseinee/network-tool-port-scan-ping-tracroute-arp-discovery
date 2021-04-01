#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<sys/socket.h>
#include<errno.h> 
#include<netdb.h> 
#include<arpa/inet.h>

const int MAX_HOSTNAME_LEN=100;

void remove_cr(char *str){
	for(int i=0;i<strlen(str); i++){
		if(str[i] == '\n'){
			str[i] = '\0';
			return;
		}
	}
}


int main()
{
	int sockfd;  
	struct addrinfo hints, *results, *p;
	struct sockaddr_in *ip_access;
	int rv;
	char *hostname = malloc(sizeof(char)*MAX_HOSTNAME_LEN);
	char ip[100], hostname_dead[100];

	printf("Enter a Domain Name: \n");
//	scanf("%s",hostname);
        fgets(hostname,MAX_HOSTNAME_LEN,stdin);

 	remove_cr(hostname);
	//not good practice
	printf("before %s  strcpy: \n",hostname);
//	strcpy(hostname,hostname);
//	strncpy(hostname,hostname,100);


	printf("after strcpy: \n");
	puts(hostname);
	//zero out structure
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;

	if ( (rv = getaddrinfo( hostname , "domain" , &hints ,  &results)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	//loops through structure using ai_next
	for(p = results; p != NULL; p = p->ai_next) 
	{
		//pass data into sockaddr_in struct
		ip_access = (struct sockaddr_in *) p->ai_addr;
		printf("IP address is %s: \n",inet_ntoa( ip_access->sin_addr ) );	

	}

	freeaddrinfo(results); 

	printf("\n");


}


/*
int main(){
	int sockfd;
	struct addrinfo hints,*results, *p;
	struct sockaddr_in *ip_access;
	int rv;
	char *hostname;
	char ip[100], inputval[100];
	
	puts("enter a domain: \n");
	fgets(inputval,strlen(inputval),stdin);

	strcpy(hostname,inputval);

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((rv = getaddrinfo(hostname, "domain", &hints, &results))){

//		fprintf(stderr, "getaddrinfo: %s\n",gai_sterror(rv));
		printf("getaddrinfo failed");
		return 1;
	}


	for(p=results; p!= NULL; p = p->ai_next){
	
		ip_access = (struct sockaddr_in *) p->ai_addr;
		
		printf("IP address is %s: \n",inet_ntoa(ip_access->sin_addr));
	}

	freeaddrinfo(results);


	printf("\n");

	return 0; 
}
*/
