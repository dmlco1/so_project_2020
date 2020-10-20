#!/bin/bash
if [ -f medicos.txt ]; then
		
	while read line; do
		rating=$(echo $line | cut -d';' -f5)		
		consultas=$(echo $line | cut -d';' -f6)

		if [ $rating -lt 5 ] && [ $consultas -gt 6 ]; then
			echo $line >> lista_negra_medicos.txt
		fi		

		done < medicos.txt
else
	echo "O ficheiro medicos.txt não existe"
fi
