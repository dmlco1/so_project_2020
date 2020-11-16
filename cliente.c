#include<stdio.h>
#include<stdlib.h>

//#define PEDIDO_CONSULTA_FILE "PedidoConsulta.txt";

typedef struct{

	int tipo; // tipo de consulta: 1-Normal, 2-COVID-19, 3-Urgente
	char descricao[100]; //descricao da consulta
	int pid_consulta; // PID do processo que quer fazer a consulta

}Consulta;

void pedir_consulta(Consulta c){

	FILE *fp; 
	fp = fopen("PedidoConsulta.txt", "w");
	//printf("OLA!\n");
    fprintf(fp, "%d\n", c.tipo); //escrever no ficheiro a consulta
    fprintf(fp, "%s\n", c.descricao);
	fprintf(fp, "%d\n", c.pid_consulta);
	fclose(fp);
	
}

int main(){
	Consulta c;
	printf("Introduza o tipo de consulta:\n");
	scanf("%d", &c.tipo);

	printf("Introduza a descricao da Consulta: \n");
	scanf("%s", c.descricao);

	c.pid_consulta = getpid();

	printf("Tipo: %d, Descricao: %s, ID= %d\n", c.tipo, c.descricao, c.pid_consulta);

	pedir_consulta(c);

}
