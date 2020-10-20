#!/bin/bash
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
	echo "Introduza os argumentos (Nome, nº de cédula profissional, especialidade médica, e-mail)"
	read args
	./cria_medico.sh $args
	;; 
	3)
	echo "Introduza os argumentos (Localidade, Saldo mínimo)"
	read args
	./stats.sh $args
	;;
	4)
	./avalia_medicos.sh
	;;
	0)
	exit
	;;
esac
