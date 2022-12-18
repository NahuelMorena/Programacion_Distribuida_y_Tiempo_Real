#!/bin/bash

#apaga y carga el vagrantfile
vagrant halt
#vagrant reload --provision
#inicia el vagrant 
vagrant up

ip_server="192.168.0.201"
echo "Se inicia server.sh"
#Ejecuta el script del server
vagrant ssh vm1 -c "bash server.sh" &
#Espera a que el anterior comando se inicie correctamente
sleep 10
#Ejecuta el script del cliente
echo "Se inicia client.sh"
vagrant ssh vm2 -c "bash client.sh ${ip_server}"
echo "finalizan las transferencias"
#Transfiere los resultados a la carpeta compartida
echo "Se copia el informe en la carpeta compartida"
vagrant ssh vm2 -c "cp 'informe.txt' '/vagrant/codigo/informe_dos_maquinas_virtuales.txt'"
