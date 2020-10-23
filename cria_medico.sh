#!/bin/bash
medico="$1;$2;$3;$4;0;0;0"
echo "$medico"
if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ]; then
	echo "Argumentos insuficientes"
	exit
fi

if [ -f medicos.txt ]; then
	linhas=$(cat medicos.txt | cut -d';' -f2 | grep "$2" | wc -l)
	if [ $linhas -eq 0 ]; then
		echo $medico | sed 's/"//g'>> medicos.txt	
	else
		echo "Este médico já existe"
	fi
	
	cat medicos.txt

else
	echo "$1;$2;$3;$4;0;0;0" >> medicos.txt
	cat medicos.txt
fi
