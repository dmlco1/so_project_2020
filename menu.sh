#!/bin/bash

while $true; do
	echo "1.Cria pacientes"
	echo "2.Cria médicos"
	echo "3.Stats"
	echo "4.Avalia médicos"
	echo "0.Sair"
	read input
	
	if [ -z $input ]; then
		echo "Tem que inserir um argumento"
	fi

	case $input in
		1)
		./cria_pacientes.sh
		;;
		2)
		echo "Introduza o Nome:"
		read nome
		echo "Introduza a cédula:"
		read cedula
		echo "Introduza a especialidade:"
		read especialidade
		echo "Introduza o email:"
		read email
		./cria_medico.sh "$nome" "$cedula" "$especialidade" "$email"
		;; 
		3)
		echo "Introduza a localidade"
		read localidade
		echo "Introduza o Saldo mínimo"
		read saldo
		./stats.sh $localidade $saldo
		;;
		4)
		./avalia_medicos.sh
		;;
		0)
		exit
		;;
	esac
done
