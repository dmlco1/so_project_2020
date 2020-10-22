#!/bin/bash
if [ -f medicos.txt ]; then

	if [ -f lista_negra_medicos.txt ]; then

		cedulas=$(cat lista_negra_medicos.txt | cut -d';' -f2)
	
		while read line; do
			rating=$(echo $line | cut -d';' -f5)		
			consultas=$(echo $line | cut -d';' -f6)	
			cedula=$(echo $line | cut -d';' -f2)
			repetidas=$(echo $cedulas | grep "$cedula" | wc -l)		
	
			if [ $repetidas -eq 0 ]; then
				if [ $rating -lt 5 ] && [ $consultas -gt 6 ]; then
					echo $line >> lista_negra_medicos.txt
				fi
			fi

		done < medicos.txt
	else
		echo "O ficheiro lista_negra_medicos.txt não existe"
	fi
else
	echo "O ficheiro medicos.txt não existe"
fi
