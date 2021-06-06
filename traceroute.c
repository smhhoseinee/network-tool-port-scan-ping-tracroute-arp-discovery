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
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>


// Define the Packet Constants
// ping packet size
//#define PING_PKT_S 64
int  PING_PKT_S = 64;

// Automatic port number
#define PORT_NO 0

// Automatic port number
#define PING_SLEEP_RATE 1000000

// Gives the timeout delay for receiving packets
// in seconds
//#define RECV_TIMEOUT 1
int RECV_TIMEOUT = 1;
int MAX_TRY= 3;
int STARTING_TTL = 1;
int FINAL_TTL = 6;
bool CONTINUE_TRACE = true;

const int PACKET_MIN_SIZE = 10;
const int MAX_PORT_STR_LEN = 7;
// Define the Ping Loop
int pingloop=4;


// ping packet structure
struct ping_pkt{
	struct icmphdr hdr;
	//char msg[PING_PKT_S-sizeof(struct icmphdr)];
	//char *msg = malloc(PING_PKT_S-sizeof(struct icmphdr));
	char *msg;
};

// Calculating the Check Sum
unsigned short checksum(void *b, int len){
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}


int atoint(char s[]){
	int i,n=0;
	for(i=0;s[i]>='0' && s[i]<='9';i++){
		n=10*n+(s[i]-'0');
	}
	return n;
}


// Interrupt handler
void intHandler(int dummy){
	pingloop=0;
	CONTINUE_TRACE = false;
}

// Performs a DNS lookup
char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con){
	printf("\nResolving DNS..\n");
	struct hostent *host_entity;
	char *ip=(char*)malloc(NI_MAXHOST*sizeof(char));
	int i;

	if ((host_entity = gethostbyname(addr_host)) == NULL){
		// No ip found for hostname
		return NULL;
	}

	//filling up address structure
	strcpy(ip, inet_ntoa(*(struct in_addr *)
				host_entity->h_addr));

	(*addr_con).sin_family = host_entity->h_addrtype;
	(*addr_con).sin_port = htons (PORT_NO);
	(*addr_con).sin_addr.s_addr  = *(long*)host_entity->h_addr;

	return ip;

}

// Resolves the reverse lookup of the hostname
char* reverse_dns_lookup(char *ip_addr){
	struct sockaddr_in temp_addr;
	socklen_t len;
	char buf[NI_MAXHOST], *ret_buf;

	temp_addr.sin_family = AF_INET;
	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	len = sizeof(struct sockaddr_in);

	if (getnameinfo((struct sockaddr *) &temp_addr, len, buf,
				sizeof(buf), NULL, 0, NI_NAMEREQD)){
//		printf("Could not resolve reverse lookup of hostname\n");
		return NULL;
	}
	ret_buf = (char*)malloc((strlen(buf) +1)*sizeof(char) );
	strcpy(ret_buf, buf);
	return ret_buf;
}

// make a ping request
void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr,
		char *ping_dom, char *ping_ip, char *rev_host, int ttl_val){

	int msg_count=0, i, addr_len, flag=1,msg_received_count=0;
	int number_of_tries = 0;

	struct ping_pkt pckt;

	struct timespec time_start, time_end, tfs, tfe;
	long double rtt_msec=0, total_msec=0;
	struct timeval tv_out;
	tv_out.tv_sec = RECV_TIMEOUT;
	tv_out.tv_usec = 0;

	clock_gettime(CLOCK_MONOTONIC, &tfs);

	// set socket options at ip to TTL and value to 64,
	// change to what you want by setting ttl_val
	if (setsockopt(ping_sockfd, SOL_IP, IP_TTL,&ttl_val, sizeof(ttl_val)) != 0){
		printf("\nSetting socket options to TTL failed!\n");
		return;
	}else{
//		printf("\nSocket set to TTL..\n");
	}

	// setting timeout of recv setting
	//    setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO,(const char*)&tv_out, sizeof tv_out);

	// send icmp packet in MAX_TRY times
	pingloop = MAX_TRY;
	while(pingloop){

		char *reverse_hostname;
		pingloop--;
		number_of_tries++;


		// flag is whether packet was sent or not
		flag=1;

		//filling packet
		bzero(&pckt, sizeof(pckt));

		pckt.hdr.type = ICMP_ECHO;
		pckt.hdr.un.echo.id = getpid();

		int packet_size = PING_PKT_S-sizeof(struct icmphdr);
		//pckt.msg = malloc(PING_PKT_S-sizeof(struct icmphdr));
		pckt.msg = malloc(packet_size);

		if(pckt.msg == NULL){
			fputs("memory allocation for msg failed",stderr);
			exit(EXIT_FAILURE);
		}

		//for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
		for ( i = 0; i < packet_size -1; i++ )
			//pckt.msg[i] = i+'0';
			pckt.msg[i] = i%10+'0';


		pckt.msg[i] = 0;
		pckt.hdr.un.echo.sequence = msg_count++;
		pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));


		usleep(PING_SLEEP_RATE);

		//send packet
		clock_gettime(CLOCK_MONOTONIC, &time_start);
		if ( sendto(ping_sockfd, &pckt, sizeof(pckt), 0,
					(struct sockaddr*) ping_addr,
					sizeof(*ping_addr)) <= 0){
			printf("\nPacket Sending Failed!\n");
			flag=0;
		}

		struct sockaddr_in r_addr;
		//receive packet
		addr_len=sizeof(r_addr);
		//        struct sockaddr_in from;


		if ( recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0,(struct sockaddr*)&r_addr, &addr_len) <= 0 && msg_count>1){
			printf("\nPacket receive failed!\n");
		}else{
			clock_gettime(CLOCK_MONOTONIC, &time_end);

			double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec))/1000000.0;
			rtt_msec = (time_end.tv_sec-time_start.tv_sec) * 1000.0+ timeElapsed;

			// if packet was not sent, don't receive
			if(flag){
				if(!(pckt.hdr.type ==69 && pckt.hdr.code==0)){
					printf("Error..Packet received with ICMP type %d code %d\n", pckt.hdr.type, pckt.hdr.code);
				}else{
					pingloop=0;
					reverse_hostname = reverse_dns_lookup(inet_ntoa(r_addr.sin_addr));
					printf("HOP<%d> <==> <%s>(reverse hosntame = <%s>) in %Lfms after %d tries \n",
						       	ttl_val, inet_ntoa(r_addr.sin_addr),  reverse_hostname, rtt_msec, number_of_tries);
					msg_received_count++;
				}
			}
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &tfe);

	double timeElapsed = ((double)(tfe.tv_nsec - tfs.tv_nsec))/1000000.0;

	total_msec = (tfe.tv_sec-tfs.tv_sec)*1000.0 + timeElapsed;

}

// Driver Code
int main(int argc, char *argv[]){
	int sockfd;
	char *ip_addr, *reverse_hostname;
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	char net_buf[NI_MAXHOST];

	in_port_t server_port;

	if (strcmp(argv[1], "-h" ) == 0 || strcmp(argv[1], "--help" ) == 0  )
	{
		printf(" -m or --maxtry : MAX TRY\n");
		printf(" -b or --bttl: beginning ttl value\n");
		printf(" -f or --fttl: final ttl value\n");
		printf(" -p or --port: sending port number\n");
		printf(" -t or --timeout: timeout(maximum waiting time)\n");
		printf(" -s or --size: size of each packet\n");
		return 0;
	}


	if(argc < 3){
		printf("\nFormat %s <address> <options>\n", argv[0]);
		printf("use -h to see more \n");
		return 0;
	}



	for(int i=1; i<argc; i++){

		printf("\n\ni=%d\n",i);
		printf("argv%d=%s\n",i,argv[i]);

		if (i+1 != argc)
		{
			if (strcmp(argv[i], "-m") == 0 ||strcmp(argv[i], "--maxtry") == 0  ) // This is your paramet  er name
			{
				MAX_TRY = atoi(argv[i+1]);
				printf("MAX TRY set to %d\n", MAX_TRY);
				i++;    // Move to the next flag
			}else if (strcmp(argv[i], "-b" ) == 0 || strcmp(argv[i], "--bttl" ) == 0  )
			{
				STARTING_TTL = atoi(argv[i+1]);
				printf("STARTING_TTL set %d\n", STARTING_TTL);
				i++;    // Move to the next flag
			}else if (strcmp(argv[i], "-f" ) == 0 || strcmp(argv[i], "--fttl" ) == 0  )
			{
				FINAL_TTL = atoi(argv[i+1]);
				printf("FINAL_TTL set to %d\n", FINAL_TTL);
				i++;    // Move to the next flag
			}else if (strcmp(argv[i], "-p" ) == 0 || strcmp(argv[i], "--port" ) == 0  )
			{
				server_port = atoi(argv[i+1]);
				addr_con.sin_port = server_port;
				printf("PORT set to %d\n", server_port);
				i++;    // Move to the next flag
			}else if (strcmp(argv[i], "-t" ) == 0 || strcmp(argv[i], "--timeout" ) == 0  )
			{
				RECV_TIMEOUT = atoi(argv[i+1]);
				printf(" TIMEOUT set to %d\n", RECV_TIMEOUT);
				i++;    // Move to the next flag
			}else if (strcmp(argv[i], "-s") == 0 ||strcmp(argv[i], "--size") == 0  ) // This is your parameter name
			{
				int input_packet_size = atoi(argv[i+1]);
				if(input_packet_size > PACKET_MIN_SIZE){
					PING_PKT_S = input_packet_size;
				}
				printf("packetsize chosen=%d\n",PING_PKT_S);
				i++;    // Move to the next flag

			}


		}
	}



	ip_addr = dns_lookup(argv[1], &addr_con);
	if(ip_addr==NULL){
		printf("\nDNS lookup failed! Could not resolve hostname!\n");
		return 0;
	}

	reverse_hostname = reverse_dns_lookup(ip_addr);
	printf("\nTrying to connect to '%s' IP: %s\n",argv[1], ip_addr);
	printf("\nReverse Lookup domain: %s",reverse_hostname);

	//socket()
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sockfd<0){
		printf("\nSocket file descriptor not received!!\n");
		return 0;
	}else{
		printf("\nSocket file descriptor %d received\n", sockfd);
	}

	signal(SIGINT, intHandler);//catching interrupt

	//int ttl = atoint(argv[2]);
	//send pings continuously
	for(int ttl = STARTING_TTL; ttl <= FINAL_TTL && CONTINUE_TRACE; ttl++){
		send_ping(sockfd, &addr_con, reverse_hostname,ip_addr, argv[1],ttl);
	}
	return 0;
}
