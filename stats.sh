#!/bin/bash
countmedicos=0
if [ -z "$1" ] || [ -z "$2" ]; then
	echo "Argumentos insuficientes"
	exit
fi

if [ -f medicos.txt ] || [ -f pacientes.txt ]; then

	numeropacientes=$(cat pacientes.txt | cut -d';' -f3 | grep $1 | wc -l)
	cat medicos.txt | cut -d';' -f7 > aux.txt	

	while read line
	do	
		if [ $line -gt $2 ]; then
			countmedicos=$(($count+1))
		fi

	done < aux.txt
	rm aux.txt

	echo "Pacientes: $numeropacientes; Médicos : $countmedicos"

else
	echo "Um dos ficheiros não é válido"
fi
