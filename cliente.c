#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>

#include "consulta.h" //include the typedef of Consulta, defined in file consulta.h

int waiting_list = 0;
int n;

int verificar_ficheiro_pedido_consultas_existe(){

    FILE *file;

    file = fopen("PedidoConsulta.txt", "r");

    //if the file is empy that means that it didn't existed before the fopen call
    if(file == NULL){
        waiting_list = 1;
		return 0;
	}
	fclose(file);
    return 1;
}

void pedir_consulta(Consulta c){

	FILE *file; 
	file = fopen("PedidoConsulta.txt", "w");

	//Escrever os dados no ficheiro PedidoConsulta.txt
    fprintf(file, "%d,%s,%d\n", c.tipo, c.descricao, c.pid_consulta); 
    //fprintf(file, "%s\n", c.descricao);
	//fprintf(file, "%d\n", c.pid_consulta);
	
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

//After receiving signal SIGHUP - delete file PedidoConsulta.txt
void consulta_iniciada(){
    printf("Consulta iniciada para o processo: %d\n", getpid());
	
	//remove file PedidoConsulta.txt
	//remove("PedidoConsulta.txt");
	printf("PedidoConsulta.txt removido com sucesso!\n");

	n = 1;
	pause();
}

//After receiving signal SIGTERM
void consulta_terminada(){
	
		//Check if signal SIGHUP has already been sent
		if(n == 1){
			printf("Consulta concluida para o processo: %d\n", getpid());
			//n = 2;
			remove("PedidoConsulta.txt");
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

	if(waiting_list == 0){
		printf("\nA tentar enviar sinal\n");
	}
}

int main(){
	Consulta c;

	//c1) Introduzir os dados
	printf("Introduza o tipo de consulta (1: Normal; 2: COVID-19; 3: Urgente):\n");
	scanf("%d", &c.tipo);

	if(c.tipo != 1 && c.tipo != 2 && c.tipo != 3){
		printf("Tipo Invalido!\n");
		exit(0);
	}

	// Ler Descricao e garantir que le mesmo que tenha espacos em branco
	char descricao[100];
	char temp;	
	printf("Introduza a descricao da Consulta:\n");
	scanf("%c", &temp);
	
	scanf("%[^\n]", descricao);	

	memcpy(c.descricao,descricao,100);

	//PID consulta e o PID deste processo
	c.pid_consulta = getpid();

	printf("Tipo: %d, Descricao: %s, PID= %d\n", c.tipo, c.descricao, c.pid_consulta);

	//Check if PedidoConsulta.txt already exists
	while(verificar_ficheiro_pedido_consultas_existe() != 0){
        
        printf("\nO ficheiro ja existe... espere uns segundos...\n");
        
        signal(SIGALRM, sinal_alarme);

		printf("\nEm espera...\n");
		alarm(10);		

		pause();
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
	//if(n == 1){
	//	pause();
	//}

	printf("-> Sinal recebido!!\n");	
	return 0;	




}
