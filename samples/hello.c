#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
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



unsigned long PING_PKT_S = 30;

int main(){

	printf("%lu\n", PING_PKT_S-sizeof(struct icmphdr));
	int diff = PING_PKT_S-sizeof(struct icmphdr);
	printf("%d\n",diff);

//	char *msg = malloc((PING_PKT_S-sizeof(struct icmphdr)));
	char *msg = malloc(diff);

	printf("%lu\n",sizeof(struct icmphdr));
	printf("%lu\n",sizeof(msg));

	for ( int i = 0; i < diff; i++ )
		msg[i] = (i%10)+'0';

	puts(msg);

	return 0;
}




