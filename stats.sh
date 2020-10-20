#!/bin/bash
countmedicos=0
if [ -z "$1" ] || [ -z "$2" ]; then
	echo "Argumentos insuficientes"
	exit
fi

if [ -f medicos.txt ] || [ -f pacientes.txt ]; then

	numeropacientes=$(cat pacientes.txt | cut -d';' -f3 | grep $1 | wc -l)	

	while read line
	do	
		linha=$(echo $line | cut -d';' -f7)

		if [ $linha -gt $2 ]; then
			countmedicos=$(($count+1))
		fi

	done < medicos.txt

	echo "Pacientes: $numeropacientes; Médicos : $countmedicos"

else
	echo "Um dos ficheiros não é válido"
fi
