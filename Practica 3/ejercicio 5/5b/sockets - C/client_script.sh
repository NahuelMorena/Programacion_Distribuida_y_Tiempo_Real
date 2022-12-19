#!/bin/bash
if [ $# -eq 0 ]; then
	echo "Bad arguments"
	exit -1
else
	IP_SERVER=$1
fi
PORT=8000
echo "Create the ouput.txt file"
> output.txt
#COMPILE CLIENT PROCESS
echo "Compiling the client process"
gcc -o client client.c
echo "It's done."
echo "Starting transfers..."
for bytes in 10 100 1000 10000 100000 1000000; do
	echo "Starting transfer of ${bytes} bytes";
	#RUN CLIENT PROCESS
	echo "Starting the client process"
	./client ${IP_SERVER} ${PORT} ${bytes} >> output.txt
	let "PORT += 1"
	echo "Transfer's done."
done;
echo "Test finish, please view the output.txt file."
