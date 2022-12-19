#!/bin/bash
PORT=8000
#COMPILE SERVER PROCESS
echo "Compiling the server process"
gcc -o server server.c
echo "It's done."
echo "Starting transfers..."
for bytes in 10 100 1000 10000 100000 1000000; do
	echo "Starting transfer of ${bytes} bytes";
	#RUN SERVER PROCESS
	echo "Starting the server process at port ${PORT}"
	./server ${PORT} ${bytes}
	let "PORT += 1"
	echo "Transfer's done."
done;
echo "Test finish, please view the output.txt file."