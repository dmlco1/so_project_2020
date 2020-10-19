#!/bin/bash
cedula=$2
medico="$1;$2;$3;$4;;;"
i=0

if [ -f medicos.txt ]; then

	cat medicos.txt | cut -d';' -f2 | while read line ; do
		n=$(cat medicos.txt | wc -l)
		echo "linhas: $n"
		i=$((i+1))
		if [ "$line" -eq "$cedula" ]; then
			echo "Este médico já existe no ficheiro"
			break
		else
			if [ $n -eq $i ]; then
				echo $medico >> medicos.txt
			else
				continue
			fi
		fi
	done

else
	echo $medico >> medicos.txt
fi
