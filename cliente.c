#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

int waiting_list;
int n;

typedef struct{

	int tipo; // tipo de consulta: 1-Normal, 2-COVID-19, 3-Urgente
	char descricao[100]; //descricao da consulta
	int pid_consulta; // PID do processo que quer fazer a consulta

}Consulta;

int verificar_ficheiro_pedido_consultas_existe(){

    FILE *file;

    file = fopen("PedidoConsulta.txt", "r");

    //if the file is empy that means that it didn't existed before the fopen call
    if(file == NULL){
        return 0;
    }
    return 1;
}

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
	
	printf("O pid do Srv e %d\n", getpid());
	
    return pid;
}

//After receiving signal SIGHUP - delete file PedidoConsulta.txt
void consulta_iniciada(){
    printf("Consulta iniciada para o processo: %d\n", getpid());
	
	//remove file PedidoConsulta.txt
	remove("PedidoConsulta.txt");
	printf("PedidoConsulta.txt removido com sucesso!\n");

	n = 1;
}

//After receiving signal SIGTERM
void consulta_terminada(){
	
		//Check if signal SIGHUP has already been sent
		if(n == 1){
			printf("Consulta concluida para o processo: %d\n", getpid());
			n = 2;
			return;
		}

		//Error if signal SIGHUP hasn't beem sent yet
		printf("Impossivel realizar operacao\n");
		exit(0);
}

//After receiving signal SIGUSR2
void consulta_impossivel(){

    printf("Consulta impossivel para o processo: %d\n", getpid());

	//Remove file PedidoConsulta.txt
    remove("PedidoConsulta.txt");
    exit(0);
}

void cancelar_espera(){

    printf("\nConsulta cancelada para o processo: %d\n", getpid());
    
	//Remove file PedidoConsulta.txt
    remove("PedidoConsulta.txt");
    exit(0);
}


void sinal_alarme(int sinal){
	waiting_list = 1;
}

int main(){
	Consulta c;

	//c1) Introduzir os dados
	printf("Introduza o tipo de consulta:\n");
	scanf("%d", &c.tipo);

	printf("Introduza a descricao da Consulta: \n");
	scanf("%s", c.descricao);

	//PID consulta e o PID deste processo
	c.pid_consulta = getpid();

	printf("Tipo: %d, Descricao: %s, ID= %d\n", c.tipo, c.descricao, c.pid_consulta);

	//Check if PedidoConsulta.txt already exists
	if(verificar_ficheiro_pedido_consultas_existe()){
        
        printf("O ficheiro ja existe... espere uns segundos...");
        
        signal(SIGALRM, sinal_alarme);

        alarm(10);

        while(waiting_list == 0);
		printf("ok\n");
    }	


	//c2) Criar Pedido de Consulta
	pedir_consulta(c);

	
	//c3) Send signal SIGUSR1 to the server
	kill(pid_of_SrvConsultas(), SIGUSR1);
	printf("-> Sinal enviado!!\n");

	printf("PID=%d\n", getpid());
	
	//c4) 
	signal(SIGHUP, consulta_iniciada);
	
	//c5)
	signal(SIGTERM, consulta_terminada);

	//c6)
	signal(SIGUSR2, consulta_impossivel);

	//c7)
	signal(SIGINT, cancelar_espera);

	//Waiting to receive a signal
	while(n == 0){
        pause();
    }

	
	//If signal SIGHUP was sent, pause until receive signal SIGTEMP
	if(n == 1){
		pause();
	}

	printf("-> Sinal recebido!!\n");	
	return 0;	




}
