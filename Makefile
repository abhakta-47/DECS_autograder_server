#!/bin/bash

OBJS = server.o myqueue.o common.o gen_uuid.o thread_pool.o req_handler_threads.o worker_threads.o

all: server client

myqueue.o: myqueue.c
	gcc -c myqueue.c

server.o: server.c
	gcc -c server.c

common.o: common.c
	gcc -c common.c

gen_uuid.o: gen_uuid.c
	gcc -c gen_uuid.c

thread_pool.o: thread_pool.c
	gcc -c thread_pool.c

req_handler_threads.o: req_handler_threads.c
	gcc -c req_handler_threads.c

worker_threads.o: worker_threads.c
	gcc -c worker_threads.c

server: $(OBJS) constants.h
	gcc $(OBJS) -o server -luuid

client: common.o client.c constants.h
	gcc common.o -o client client.c

#run: all
#	./server  &
#	sleep 3 && ./performance_starter.sh

clean:
	rm $(OBJS) server client
	rm -f analysis.txt diff_output.txt error_output.txt out_gen.txt server_to_client_response_file.txt c_code_server.c diff_out.txt
	rm -rf client_outputs util
	pkill server
	pkill util.sh
