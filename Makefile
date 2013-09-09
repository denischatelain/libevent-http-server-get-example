all: libevent-http-server-get-example 

libevent-http-server-get-example: libevent-http-server-get-example.o 
	$(CC) -o libevent-http-server-get-example libevent-http-server-get-example.o -l event
