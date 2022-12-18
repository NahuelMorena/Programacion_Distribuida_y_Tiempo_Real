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
bash codigo/client.sh ${ip_server}
echo "finalizan las transferencias"
#Transcribe el nombre del archivo
echo "Se modifica el nombre del informe generado"
mv '/codigo/informe.txt' '/codigo/informe_maquina_virtual_y_host.txt'