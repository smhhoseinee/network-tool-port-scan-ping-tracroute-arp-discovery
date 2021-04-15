// CPP program to illustrate
// default Signal Handler
#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

void handle_child(int sig){
	printf("child terminated\n");
}

void handle_parent(int sig){
	printf("parent terminated\n");
}

int main()
{


	printf("start creating child process");
	sleep(1);
	int pid = fork();

	if (pid == 0){
		signal(SIGUSR1, handle_child);
		while (1)
		{
			printf("I'm child\n");
			sleep(1);
		}

	}else{
		signal(SIGINT, handle_child);
		while (1)
		{
			printf("I'm parent\n");
			sleep(2);
		}

	}

	return 0;
}
