/*  Copyright (C) 2011-2015  P.D. Buchan (pdbuchan@yahoo.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see &lt;http://www.gnu.org/licenses/&gt;.
    */

// Send an IPv4 ARP packet via raw socket at the link layer (ethernet frame).
// Values set for ARP request.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>           // close()
#include <string.h>           // strcpy, memset(), and memcpy()

#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_ARP = 0x0806
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>

#include <errno.h>            // errno, perror()

#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>

#include "ip_parser.h"


// Define a struct for ARP header
typedef struct _arp_hdr arp_hdr;
struct _arp_hdr {
	uint16_t htype;
	uint16_t ptype;
	uint8_t hlen;
	uint8_t plen;
	uint16_t opcode;
	uint8_t sender_mac[6];
	uint8_t sender_ip[4];
	uint8_t target_mac[6];
	uint8_t target_ip[4];
};

// Define some constants.
#define ETH_HDRLEN 14      // Ethernet header length
#define IP4_HDRLEN 20      // IPv4 header length
#define ARP_HDRLEN 28      // ARP header length
#define ARPOP_REQUEST 1    // Taken from &lt;linux/if_arp.h&gt;

#define _OPEN_SYS_ITOA_EXT

//rec part
#define ARPOP_REPLY 2         // Taken from <linux/if_arp.h>



// Function prototypes
char *allocate_strmem (int);
uint8_t *allocate_ustrmem (int);

void remove_cr(char *str){
	for(int i=0;i<strlen(str); i++){
		if(str[i] == '\n'){
			str[i] = '\0';
			return;
		}
	}
}

struct sockaddr_in create_struct_sockaddr(struct sockaddr_in server_address, char *server_addr_str, in_port_t server_port){
	//create struct sockaddr
	memset(&server_address,0,sizeof(server_address));
	server_address.sin_family = AF_INET;
	int numerical_address = inet_pton(AF_INET,server_addr_str, &server_address.sin_addr.s_addr);

	if(numerical_address == 0){
		fputs("invalid IPv4", stderr);
		exit(EXIT_FAILURE);
	}
	if(numerical_address < 0){
		fputs("p to n failed",stderr);
		exit(EXIT_FAILURE);
	}
	server_address.sin_port = htons(server_port);

	return server_address;
}

unsigned short htons ( unsigned short us )
{
	return ( ((unsigned char*)&us)[0] << 8 ) + ((unsigned char*)&us)[1];
}

void dumpbin ( unsigned char* pbyBin, int nLen )
{
	int i;
	for ( i = 0; i < nLen; ++i )
	{
		printf ( "%02x", pbyBin[i] );
	}
}


void testcase ( const char* pszTest )
{
	unsigned char abyAddr[16];
	int bIsIPv6;
	int nPort;
	int bSuccess;

	printf ( "Test case '%s'\n", pszTest );
	const char* pszTextCursor = pszTest;
	bSuccess = ParseIPv4OrIPv6 ( &pszTextCursor, abyAddr, &nPort, &bIsIPv6 );
	if ( ! bSuccess )
	{
		printf ( "parse failed, at about index %ld ; rest: '%s'\n", pszTextCursor - pszTest, pszTextCursor );
		return;
	}

	printf ( "addr:  " );
	dumpbin ( abyAddr, bIsIPv6 ? 16 : 4 );
	printf ( "\n" );
	if ( 0 == nPort )
		printf ( "port absent" );
	else
		printf ( "port:  %d", htons ( nPort ) );
	printf ( "\n\n" );

}

int main (int argc, char **argv)
{
	int i, status, frame_length, sd, bytes;
	char *interface, *start_ip_str, *src_ip, *end_ip_str;
	char *first_3_bytes ,*ip_str_iterator ;
	char* last_byte_iterator ;
	arp_hdr arphdr;
	uint8_t *src_mac, *dst_mac, *ether_frame;
	struct addrinfo hints, *res;
	struct sockaddr_in *ipv4;
	struct sockaddr_ll device;
	struct ifreq ifr;

	struct sockaddr_in addr_con;

	in_port_t start_ip_addr;
	in_port_t end_ip_addr;
	unsigned long starting_ip_long;
	int timeout_value = 3;
	int start_last_byte;
	int end_last_byte;



	// Allocate memory for various arrays.
	src_mac = allocate_ustrmem (6);
	dst_mac = allocate_ustrmem (6);
	ether_frame = allocate_ustrmem (IP_MAXPACKET);
	interface = allocate_strmem (40);
	start_ip_str = allocate_strmem (40);
	end_ip_str = allocate_strmem (40);
	first_3_bytes = allocate_strmem (40);
	ip_str_iterator = allocate_strmem (40);
	last_byte_iterator = allocate_strmem (40);
	src_ip = allocate_strmem (INET_ADDRSTRLEN);


	for(i=0;i<9;i++){
		printf("%d",i);
	}
	//	return 0;


	if(argc < 2){
		printf("\nFormat %s <address> <options>\n", argv[0]);
		printf("use -h to see more \n");
		return 0;
	}



	if (strcmp(argv[1], "-h" ) == 0 || strcmp(argv[1], "--help" ) == 0  )
	{
		printf(" -t or --timeout_value: timeout_value(maximum waiting time)\n");
		printf(" -s or --start: start ip address\n");
		printf(" -e or --end: end ip address\n");
		return 0;
	}


	if(argc < 3){
		printf("\nFormat %s <address> <options>\n", argv[0]);
		printf("use -h to see more \n");
		return 0;
	}

	printf("argc=%d\n",argc);
	for(int i=1; i<argc; i++){

		printf("\n\ni=%d\n",i);
		printf("argv%d=%s\n",i,argv[i]);

		if (i+1 != argc)
		{
			if (strcmp(argv[i], "-s") == 0 ||strcmp(argv[i], "--start") == 0  ) // This is your param  et  er name
			{
				//strcpy (start_ip_str, "212.33.204.53");
				strcpy (start_ip_str, argv[i+1]);
				remove_cr(start_ip_str);
				printf(" start_ip_str =  %s\n", start_ip_str);

				struct sockaddr_in server_address;
				int numerical_address = inet_pton(AF_INET, start_ip_str , &start_ip_addr);
				//int numerical_address2 = inet_pton(AF_INET, start_ip_str , &starting_ip_long);

				//		printf("server_ip_start : %hu\n", start_ip_addr);
				//		printf("server_ip_start_long : %lu\n", starting_ip_long);

				printf("before i++  --- \n");
				printf("i=%d\n",i);
				i++;    // Move to the next flag
				printf("after i++  --- \n");
				printf("i=%d\n",i);

			}else if (strcmp(argv[i], "-e" ) == 0 || strcmp(argv[i], "--end" ) == 0  )
			{

				strcpy (end_ip_str, argv[i+1]);
				remove_cr(end_ip_str);
				printf("end_ip_str =  %s\n", end_ip_str);

				struct sockaddr_in server_address;
				int numerical_address = inet_pton(AF_INET, end_ip_str , &end_ip_addr);

				i++;    // Move to the next flag
			}else if (strcmp(argv[i], "-t" ) == 0 || strcmp(argv[i], "--timeout_value" ) == 0  )
			{
				timeout_value = atoi(argv[i+1]);
				printf("timeout_value set to %d\n", timeout_value);
				i++;    // Move to the next flag
			}


		}

		printf("after if --- \n");
		printf("i=%d\n",i);
		//	printf("argv%d=%s\n",i,argv[i]);



	}

	// Extract the first token
	char * token = strtok(start_ip_str , ".");
	char* start_last_byte_str;
	int byte_counter = 0;
	int bytes_of_start_ip[4];
	int bytes_of_end_ip[4];
	int j = 0;
	// loop through the string to extract all other tokens
	while( token != NULL ) {
		bytes_of_start_ip[j]=atoi(token);
		j++;
		if(byte_counter < 3){
			first_3_bytes = strcat(first_3_bytes,token);
			first_3_bytes = strcat(first_3_bytes,".");
			byte_counter++;
		}
		start_last_byte_str = token;
		printf( "token  %s\n", token ); //printing each token
		token = strtok(NULL, ".");
	}

	printf( " first_3_bytes %s\n",  first_3_bytes); //printing each token
	printf( "start_lastByte %s\n", start_last_byte_str); //printing each token


	start_last_byte = atoi(start_last_byte_str);

	// Extract the second token
	printf( "end_ip_str=%s",end_ip_str); //printing each token
	char * end_token = strtok(end_ip_str , ".");
	char* end_last_byte_str;
	// loop through the string to extract all other tokens

	j = 0;
	while( end_token != NULL ) {
		bytes_of_end_ip[j]=atoi(end_token);
		j++;
		end_last_byte_str = end_token;
		printf( "token  %s\n", end_token ); //printing each token
		end_token = strtok(NULL, ".");
	}

	printf( "end_lastByte %s\n", end_last_byte_str); //printing each token

	end_last_byte = atoi(end_last_byte_str);


	printf( "from byte %d to %d\n",start_last_byte, end_last_byte); //printing each token

	// Interface to send packet through.
	//strcpy (interface, "wlan0");
	strcpy (interface, "ens160");

	// Submit request for a socket descriptor to look up interface.
	if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror ("socket() failed to get socket descriptor for using ioctl() ");
		exit (EXIT_FAILURE);
	}

	// Use ioctl() to look up interface name and get its MAC address.
	memset (&ifr, 0, sizeof (ifr));
	snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
	if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
		perror ("ioctl() failed to get source MAC address ");
		return (EXIT_FAILURE);
	}
	close (sd);

	// Copy source MAC address.
	memcpy (src_mac, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t));

	// Report source MAC address to stdout.
	printf ("MAC address for interface %s is ", interface);
	for (i=0; i<5; i++) {
		printf ("%02x:", src_mac[i]);
	}
	printf ("%02x\n", src_mac[5]);

	// Find interface index from interface name and store index in
	// struct sockaddr_ll device, which will be used as an argument of sendto().
	memset (&device, 0, sizeof (device));
	if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
		perror ("if_nametoindex() failed to obtain interface index ");
		exit (EXIT_FAILURE);
	}
	printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);

	// Set destination MAC address: broadcast address
	memset (dst_mac, 0xff, 6 * sizeof (uint8_t));



	// Source IPv4 address:  you need to fill this out
	//  strcpy (src_ip, "192.168.0.106");
	strcpy (src_ip, "212.33.204.63");

	// Destination URL or IPv4 address (must be a link-local node): you need to fill this out
	//  strcpy (start_ip_str, "192.168.0.1");
	//  strcpy (start_ip_str, "212.33.204.53");

	// Fill out hints for getaddrinfo().
	memset (&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = hints.ai_flags | AI_CANONNAME;

	// Source IP address
	if ((status = inet_pton (AF_INET, src_ip, &arphdr.sender_ip)) != 1) {
		fprintf (stderr, "inet_pton() failed for source IP address.\nError message: %s", strerror (status));
		exit (EXIT_FAILURE);
	}

	for(i = start_last_byte; i <= end_last_byte; i++){



		sprintf(last_byte_iterator, "%d", i);

		strcpy(ip_str_iterator,first_3_bytes);
		printf("ip_str_iterator= %s\n",ip_str_iterator);
		strncat(ip_str_iterator, last_byte_iterator, 3);

		printf("ip_str_iterator= %s\n",ip_str_iterator);
		//	ip_str_iterator = strcat(first_3_bytes, last_byte_iterator);
		printf("last_byte_iterator = %s\n",last_byte_iterator);
		printf("ip_str_iterator= %s\n",ip_str_iterator);
		printf("first_3_bytes= %s\n",first_3_bytes);
		//	}
		// Resolve start_ip_str using getaddrinfo().
		//	if ((status = getaddrinfo (start_ip_str, NULL, &hints, &res)) != 0) {
		if ((status = getaddrinfo (ip_str_iterator, NULL, &hints, &res)) != 0) {
			fprintf (stderr, "getaddrinfo() failed: %s\n", gai_strerror (status));
			exit (EXIT_FAILURE);
		}

		ipv4 = (struct sockaddr_in *) res->ai_addr;
		memcpy (&arphdr.target_ip, &ipv4->sin_addr, 4 * sizeof (uint8_t));
		freeaddrinfo (res);

		// Fill out sockaddr_ll.
		device.sll_family = AF_PACKET;
		memcpy (device.sll_addr, src_mac, 6 * sizeof (uint8_t));
		device.sll_halen = 6;

		// ARP header

		// Hardware type (16 bits): 1 for ethernet
		arphdr.htype = htons (1);

		// Protocol type (16 bits): 2048 for IP
		arphdr.ptype = htons (ETH_P_IP);

		// Hardware address length (8 bits): 6 bytes for MAC address
		arphdr.hlen = 6;

		// Protocol address length (8 bits): 4 bytes for IPv4 address
		arphdr.plen = 4;

		// OpCode: 1 for ARP request
		arphdr.opcode = htons (ARPOP_REQUEST);

		// Sender hardware address (48 bits): MAC address
		memcpy (&arphdr.sender_mac, src_mac, 6 * sizeof (uint8_t));

		// Sender protocol address (32 bits)
		// See getaddrinfo() resolution of src_ip.

		// Target hardware address (48 bits): zero, since we don't know it yet.
		memset (&arphdr.target_mac, 0, 6 * sizeof (uint8_t));

		// Target protocol address (32 bits)
		// See getaddrinfo() resolution of start_ip_str.

		// Fill out ethernet frame header.

		// Ethernet frame length = ethernet header (MAC + MAC + ethernet type) + ethernet data (ARP header)
		frame_length = 6 + 6 + 2 + ARP_HDRLEN;

		// Destination and Source MAC addresses
		memcpy (ether_frame, dst_mac, 6 * sizeof (uint8_t));
		memcpy (ether_frame + 6, src_mac, 6 * sizeof (uint8_t));

		// Next is ethernet type code (ETH_P_ARP for ARP).
		// http://www.iana.org/assignments/ethernet-numbers
		ether_frame[12] = ETH_P_ARP / 256;
		ether_frame[13] = ETH_P_ARP % 256;

		// Next is ethernet frame data (ARP header).

		// ARP header
		// memcpy (ether_frame + ETH_HDRLEN, &amp;arphdr, ARP_HDRLEN * sizeof (uint8_t));
		memcpy (ether_frame + ETH_HDRLEN, &arphdr, ARP_HDRLEN * sizeof (uint8_t));

		// Submit request for a raw socket descriptor.
		if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
			perror ("socket() failed ");
			exit (EXIT_FAILURE);
		}

		// Send ethernet frame to socket.
		if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
			perror ("sendto() failed");
			exit (EXIT_FAILURE);
		}


		//
		//
		//receiving 
		//
		//

		int i, sd, status;
		uint8_t *ether_frame;
		arp_hdr *arphdr;

		bool waiting = true;
		bool last_bytes[256];
		int  counter=0;

		char *received_ip_str , *received_last_byte_str;

		received_ip_str = allocate_strmem (40);
		received_last_byte_str = allocate_strmem (40);

		for(int i = 0; i < 256; i++){
			last_bytes[i] = false;
		}

		for(i = start_last_byte; i <= end_last_byte && i<=255; i++){
			last_bytes[i] = true;
			counter++;
		}

		// Allocate memory for various arrays.
		ether_frame = allocate_ustrmem (IP_MAXPACKET);

		// Submit request for a raw socket descriptor.
		// if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) &lt; 0) {
		if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
			perror ("socket() failed ");
			exit (EXIT_FAILURE);
		}
		//	int ReceiveTimeout = 3000;
		//	int e = setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char*)&ReceiveTimeout, sizeof(int));

		struct timeval timeout;      
		timeout.tv_sec = timeout_value;
		timeout.tv_usec = 0;

		if (setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
			perror("setsockopt failed\n");
		}else{
			printf ("\ntime out for setsockopt() set %d successfully\n",timeout_value);
		}

		//	if (e < 0) {
		//                perror ("setsockopt() failed ");
		//                exit (EXIT_FAILURE);
		//        }


		// Listen for incoming ethernet frame from socket sd.
		// We expect an ARP ethernet frame of the form:
		//     MAC (6 bytes) + MAC (6 bytes) + ethernet type (2 bytes)
		//     + ethernet data (ARP header) (28 bytes)
		// Keep at it until we get an ARP reply.
		arphdr = (arp_hdr *) (ether_frame + 6 + 6 + 2);



		while(waiting){
			memset (ether_frame, 0, IP_MAXPACKET * sizeof (uint8_t));
			while (((((ether_frame[12]) << 8) + ether_frame[13]) != ETH_P_ARP) || (ntohs (arphdr->opcode) != ARPOP_REPLY)) {
				if ((status = recv (sd, ether_frame, IP_MAXPACKET, 0)) < 0) {
					if (errno == EINTR) {
						memset (ether_frame, 0, IP_MAXPACKET * sizeof (uint8_t));
						continue;  // Something weird happened, but let's try again.
					} else {
						perror ("recv() failed:");
						exit (EXIT_FAILURE);
					}
				}
			}
	//		close (sd);

			//	utoa (arphdr->opcode,received_ip_str, 10);
			if(last_bytes[arphdr->sender_ip[3]] == true ){

				printf ("Sender protocol (IPv4) address: %u.%u.%u.%u\n",
						arphdr->sender_ip[0], arphdr->sender_ip[1], arphdr->sender_ip[2], arphdr->sender_ip[3]);



				strcpy(received_ip_str,first_3_bytes);
				sprintf(received_last_byte_str, "%d", arphdr->sender_ip[3]);
				strcat(received_ip_str, received_last_byte_str);


				printf ("\n ip  %s is up :\n",received_ip_str );

				last_bytes[arphdr->sender_ip[3]] = false;
				counter--;
				if(counter <= 0){
					printf ("\nfinished with %u :\n",arphdr->sender_ip[3] );
					waiting = false;
				}
				// Print out contents of received ethernet frame.
				printf ("\nEthernet frame header:\n");
				printf ("Destination MAC (this node): ");
				for (i=0; i<5; i++) {
					printf ("%02x:", ether_frame[i]);
				}
				printf ("%02x\n", ether_frame[5]);
				printf ("Source MAC: ");
				for (i=0; i<5; i++) {
					printf ("%02x:", ether_frame[i+6]);
				}
				//                printf ("%02x\n", ether_frame[11]);
				//                // Next is ethernet type code (ETH_P_ARP for ARP).
				//                // http://www.iana.org/assignments/ethernet-numbers
				//                printf ("Ethernet type code (2054 = ARP): %u\n", ((ether_frame[12]) << 8) + ether_frame[13]);
				//                printf ("\nEthernet data (ARP header):\n");
				//                printf ("Hardware type (1 = ethernet (10 Mb)): %u\n", ntohs (arphdr-> htype));
				//                printf ("Protocol type (2048 for IPv4 addresses): %u\n", ntohs (arphdr-> ptype));
				//                printf ("Hardware (MAC) address length (bytes): %u\n", arphdr-> hlen);
				//                printf ("Protocol (IPv4) address length (bytes): %u\n", arphdr->plen);
				//                printf ("Opcode (2 = ARP reply): %u\n", ntohs (arphdr->opcode));
				//                printf ("Sender hardware (MAC) address: ");
				//                for (i=0; i<5; i++) {
				//                        printf ("%02x:", arphdr->sender_mac[i]);
				//                }
				//                printf ("%02x\n", arphdr->sender_mac[5]);
				printf ("Sender protocol (IPv4) address: %u.%u.%u.%u\n",
						arphdr->sender_ip[0], arphdr->sender_ip[1], arphdr->sender_ip[2], arphdr->sender_ip[3]);
				printf ("Target (this node) hardware (MAC) address: ");
				for (i=0; i<5; i++) {
					printf ("%02x:", arphdr->target_mac[i]);
				}
				printf ("%02x\n", arphdr->target_mac[5]);
				printf ("Target (this node) protocol (IPv4) address: %u.%u.%u.%u\n",
						arphdr->target_ip[0], arphdr->target_ip[1], arphdr->target_ip[2], arphdr->target_ip[3]);

			}
	
		}

		close (sd);
		free (ether_frame);


		}


		//
		//
		//end of receiving
		//
		//


		// Close socket descriptor.
		close (sd);

		// Free allocated memory.
		free (src_mac);
		free (dst_mac);
		free (ether_frame);
		free (interface);
		free (start_ip_str);
		free (end_ip_str);
		free (first_3_bytes);
		free (ip_str_iterator);
		free (last_byte_iterator);
		free (src_ip);

		printf ("Exit Succesful");

		return (EXIT_SUCCESS);
}

// Allocate memory for an array of chars.
	char *
allocate_strmem (int len)
{
	void *tmp;

	if (len <= 0) {
		fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_strmem().\n", len);
		exit (EXIT_FAILURE);
	}

	tmp = (char *) malloc (len * sizeof (char));
	if (tmp != NULL) {
		memset (tmp, 0, len * sizeof (char));
		return (tmp);
	} else {
		fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_strmem().\n");
		exit (EXIT_FAILURE);
	}
}

// Allocate memory for an array of unsigned chars.
	uint8_t *
allocate_ustrmem (int len)
{
	void *tmp;

	if (len <= 0) {
		fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_ustrmem().\n", len);
		exit (EXIT_FAILURE);
	}

	tmp = (uint8_t *) malloc (len * sizeof (uint8_t));
	if (tmp != NULL) {
		memset (tmp, 0, len * sizeof (uint8_t));
		return (tmp);
	} else {
		fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_ustrmem().\n");
		exit (EXIT_FAILURE);
	}
}
