# computer-network-project
 <img src="https://github.com/smhhoseinee/computer-network-project/blob/main/res/logo.jpg" width="200" height="200" />

phase0 : 
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
first compile main.c

``` 
gcc main.c -lpthread 
```

run the the executable with default thread and timeout:

``` 
./a.out 
```

run with specified threads and timeout

``` 
./a.out <number_of_threads> <timeout> 
```

i.e : 3 threads running and 4 sec timeout 

```
./a.out 3 4 
```



---

### phase 2 : ping tool

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
