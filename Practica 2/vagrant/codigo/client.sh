#!/bin/bash
if [ $# -ne 1 ]; then
	echo "Se requiere un argumento para ejecutar el script"
	exit -1
fi

#Se compila el programa cliente
gcc client.c -o client -lm

#Se setean variables
ip=$1
port=4000
array=(10 100 1000 10000 100000 1000000)

#Se crea el archivo 
> informe.txt
echo -e "Estudio del tiempo promedio en la transferencia de bytes\n\n" >> informe.txt


#Comienzan las pruebas
for buffer_size in ${array[@]}; do
    echo "Pruebas de transferencia de ${buffer_size} bytes"
	if [ ${buffer_size} -ge 1000000 ]; then
		echo "La operación puede demorar..."
	fi
    ./client ${ip} ${port} ${buffer_size} >> informe.txt
	port=$(($port + 1))
done

echo "Finalizan las transferencias, resultados en informe.txt"
