#include<stdio.h>
#include<stdlib.h>

typedef struct{

	int tipo; // tipo de consulta: 1-Normal, 2-COVID-19, 3-Urgente
	char descricao[100]; //descricao da consulta
	int pid_consulta; // PID do processo que quer fazer a consulta

}Consulta;


int main(){
	Consulta c;
	printf("Introduza o tipo de consulta:\n");
	scanf("%d", &c.tipo);

	printf("Introduza a descricao da Consulta: \n");
	scanf("%s", c.descricao);

	c.pid_consulta = getpid();

	printf("Tipo: %d, Descricao: %s, ID= %d\n", c.tipo, c.descricao, c.pid_consulta);


}
