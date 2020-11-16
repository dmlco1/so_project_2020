#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

typedef struct{

	int tipo; // tipo de consulta: 1-Normal, 2-COVID-19, 3-Urgente
	char descricao[100]; //descricao da consulta
	int pid_consulta; // PID do processo que quer fazer a consulta

}Consulta;

void pedir_consulta(Consulta c){

	FILE *file; 
	file = fopen("PedidoConsulta.txt", "w");
	
	//Escrever os dados no ficheiro PedidoConsulta.txt
    fprintf(file, "%d\n", c.tipo); 
    fprintf(file, "%s\n", c.descricao);
	fprintf(file, "%d\n", c.pid_consulta);
	
	fclose(file);
}

//Get the pid of the server, writen in file SrvConsultas.pid
int pid_of_SrvConsultas(){
    int pid;

    FILE *file;
    file = fopen("SrvConsultas.pid", "r");
	
	//if the file does not exists yet
    if(file == NULL){
        printf("O ficheiro SrvConsultas.pid nao existe\n");
        exit(0);
    }
	
	//read the pid of the server
    fscanf(file, "%d", &pid);

    fclose(file);
	
	printf("O pid do Srv e %d\n", pid);
	
    return pid;
}


int main(){
	Consulta c;
	
	//Introduzir os dados
	printf("Introduza o tipo de consulta:\n");
	scanf("%d", &c.tipo);

	printf("Introduza a descricao da Consulta: \n");
	scanf("%s", c.descricao);

	//PID consulta e o PID deste processo
	c.pid_consulta = getpid();

	printf("Tipo: %d, Descricao: %s, ID= %d\n", c.tipo, c.descricao, c.pid_consulta);

	//Criar Pedido de Consulta
	pedir_consulta(c);
	printf("PID=%d\n", getpid());
	
	//Send signal SIGUSR1 to the server
	kill(pid_of_SrvConsultas(), SIGUSR1);
	printf("-> Sinal enviado!!\n");
	
	return 0;	




}
