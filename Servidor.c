#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include "consulta.h"

int count_consulta_1;
int count_consulta_2;
int count_consulta_3;
int count_consulta_perdida;

//Create file SrvConsultas
void escrever_Pid_SrvConsultas(){

    FILE *file;
    file = fopen("SrvConsultas.pid", "w");

    //Escrever os dados no ficheiro
    fprintf(file, "%d\n", getpid());

    fclose(file);
}

int main(){

	Consulta lista_consultas[10];

	for(int i = 0; i < 10; i++){
		lista_consultas[i].tipo = -1;
	}

	//Create file SrvConsultas
	escrever_Pid_SrvConsultas();
}
