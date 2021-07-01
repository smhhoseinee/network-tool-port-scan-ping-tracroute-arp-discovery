# computer-network-project
 <img src="https://github.com/smhhoseinee/computer-network-project/blob/main/res/logo.jpg" width="200" height="200" />

### phase0 : 
a report about 3 items :
- packet sniffing
- packet analyzing
- available libraries to develope them in C for linux

you have  2  ways to view or download the report doc file:

1- it is available in this repo under the name and link  [packet sniffing vs packet analyzing.pdf file](https://github.com/smhhoseinee/computer-network-project/blob/46aa40263d9e9d57702af41da1743bfb22b3134e/packet%20sniffing%20vs%20packet%20analyzing.pdf) 

https://github.com/smhhoseinee/computer-network-project/blob/46aa40263d9e9d57702af41da1743bfb22b3134e/packet%20sniffing%20vs%20packet%20analyzing.pdf



2- this file is available in ondrive under the  [link](https://1drv.ms/b/s!Ai-wUdAAvS-DgcVehvLHy1M-5l5ZQw) below: 

https://1drv.ms/b/s!Ai-wUdAAvS-DgcVehvLHy1M-5l5ZQw


also this file is available in ondrive/github in docs format -ondrive address :   [link](https://onedrive.live.com/view.aspx?resid=832FBD00D051B02F!25155&ithint=file%2cdocx&authkey=!ACf0g1mH2M2xNzc) below: 

https://onedrive.live.com/view.aspx?resid=832FBD00D051B02F!25155&ithint=file%2cdocx&authkey=!ACf0g1mH2M2xNzc


---

I chose C language for this project because I has the ability to open RAW SOCKETs also we could implement and use heap on it.
C is a recommended language for developing in lower layers of network projects.


---

### phase 1 : port scanner


in this phase we created a port scanner wich uses socket connection to check wether a port is open or not

how to use : 
first compile `port_scanner.c`  

``` 
gcc port_scanner.c -o port_scanner -lpthread
```

run the the executable with default thread and timeout:

``` 
./port_scanner 
```

run with specified threads and timeout

``` 
./port_scanner <number_of_threads> <timeout> 
```

i.e : 3 threads running and 4 sec timeout 

```
./port_scanner 3 4 
```



---

### phase 2 : ping tool

first compile `ping.c`  

``` 
gcc ping.c -o ping -lpthread
```

you could ping each domain or ip using this tool

```
./ping <hostname>
./ping github.com
```

ping multiple hostss parallel:
```
./ping google.com Instagram.com 8.8.8.8 github.com 
```

### flags:
``-s , --size`` set size of packets to send (default value is 64)
``-t , --timeout`` set time wait for response (default is 1 second)


``-s`` example :
```
./ping -s <size> <hostname>
./ping -s 126 google.com
```

``-t`` example :
```
./ping -t <timeout> <hostname>
./ping -t 5 google.com
```

a complicated example:
```
./ping google.com -t 4 Instagram.com 8.8.8.8 github.com -s 120 
```

---

### phase 3 : traceroute tool

first compile `traceroute.c`  

``` 
gcc traceroute.c -o traceroute -lpthread
```

you could traceroute each domain or ip using this tool

```
./traceroute <hostname>
./traceroute github.com
```

to see more just you could use -h 

```
./traceroute -h
```



### flags:


``-s , --size`` set size of packets to send (default value is 64)

``-t , --timeout`` set time wait for response (default is 1 second)

``-m , --maxtry `` set MAX TRY

``-b , --bttl`` beginning ttl value

``-f , --fttl`` final ttl value

``-p , --port`` sending port number

``-t , --timeout`` timeout(maximum waiting time)

``-s , --size`` size of each packet



``-s`` example :
```
./traceroute  <hostname> -s <size>
./traceroute  google.com -s 126
```

``-t`` example :
```
./traceroute <hostname> -t <timeout> 
./traceroute google.com -t 5 
```

a complicated example:
```
./traceroute google.com -s 15  -b 1 -f 64
```


---

### phase 4 : host discovery using arp

first compile the code   

``` 
gcc -Wall send_arp_request_1_by_1.c ip_parser.c  -o send_arp_request_1_by_1
```

you could send arp requests to a range of ip addresses using this tool

```
./send_arp_request_1_by_1 -s <starting ip> -e <ending(last) ip>
 ./send_arp_request_1_by_1 -s 212.33.204.120 -e 212.33.204.125 -t 4
```

to see more just you could use -h 

```
./send_arp_request_1_by_1 -h
```



### flags:


``-s , --start`` set starting ip address

``-t , --timeout_value`` set time wait for response (default is 1 second)

``-e , --end `` set end ip address


a complete example:
```
 ./send_arp_request_1_by_1 -s 192.168.1.10 -e 192.168.1.20 -t 4
```

