#!/bin/bash

#Se compila el programa servidor
gcc server.c -o server

#Se setean variables
array=(10 100 1000 10000 100000 1000000)
port=4000

#Comienzan las pruebas
for buffer_size in ${array[@]}; do
    echo "Pruebas de transferencia de ${buffer_size} bytes..."
    ./server ${port} ${buffer_size}
    port=$(($port + 1))
done

echo "Finalizan las trasnferencias al servidor"
