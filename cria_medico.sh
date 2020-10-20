#!/bin/bash
medico="$1;$2;$3;$4;0;0;0"

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ]; then
	echo "Argumentos insuficientes"
	exit
fi

if [ -f medicos.txt ]; then
	linhas=$(cat medicos.txt | cut -d';' -f2 | grep "$2" | wc -l)
	if [ $linhas -eq 0 ]; then
		echo $medico >> medicos.txt	
	else
		echo "Este médico já existe"
	fi

else
	echo $medico >> medicos.txt
fi
