#!/bin/bash

OBJS = server.o myqueue.o

all: $(OBJS) client
	gcc $(OBJS) -o server

myqueue.o: myqueue.c
	gcc -c myqueue.c

server.o: server.c
	gcc -c server.c


client: client.c
	gcc -o client client.c

#run: all
#	./server  &
#	sleep 3 && ./performance_starter.sh

clean:
	rm $(OBJS) server
	rm -f analysis.txt diff_output.txt error_output.txt out_gen.txt server_to_client_response_file.txt c_code_server.c diff_out.txt
	rm -rf client_outputs util
	pkill server
	pkill util.sh
