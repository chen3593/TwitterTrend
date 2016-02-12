Description:

TwitterTrend server is a simple program that takes city names from client programs and serves top 3 trending keywords to client programs. The server is serving multiple concurrent clients in a pool of threads. One thread on the server program handles a client program. TwitterTrend client sends the request to get the 3 trending keywords from the server. This client program have a clientX.in file that contains one or multiple city names. In addition, the communication protocol(messages) should be implemented to send and receive the data through the network. 

The purpose of this program:

I implemented a multi-thread twitterTrend and twitterTrend client to do network programming.

How to compile the program:
    
You may simply use the makefile (type make) to compile both twitterTrend_server.c twitterTrend_client.c 

How to use the program from the shell (syntax):

```   
./twitterTrend_server 2345 2   //(in machine 0)
./twitterTrend_client 127.0.0.1 2345 client1.in   //(in machine 1 or terminal 1)
./twitterTrend_client 127.0.0.1 2345 client2.in   //(in machine 2 or terminal 2)
```

What exactly the program does:
    
This is a client_server twitterTrend program. twitterTrend server is a program that takes the city names from client program requests and serves top 3 trending keywords to client program.

