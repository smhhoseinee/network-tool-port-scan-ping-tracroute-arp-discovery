#include<stdio.h>
#include<signal.h>
#include<wait.h>
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

int val = 10;
void handler(int sig)
{
	val += 5;
}
int main()
{
	pid_t pid;
	signal(SIGCHLD, handler);
	if ((pid = fork()) == 0)
	{
		val -= 3;
		exit(0);
	}
	waitpid(pid, NULL, 0);
	printf("val = %d\n", val);
	exit(0);
}
