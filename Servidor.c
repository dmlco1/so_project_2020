#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include "consulta.h"

int count_consulta_1;
int count_consulta_2;
int count_consulta_3;
int count_consulta_perdida;

int n_Alarme;
int n;

Consulta *lista_consultas=NULL;
int *indice_lista_consultas = NULL;


//Create file SrvConsultas
void escrever_Pid_SrvConsultas(){

    FILE *file;
    file = fopen("SrvConsultas.pid", "w");

    //Escrever os dados no ficheiro
    fprintf(file, "%d\n", getpid());

    fclose(file);
}

//mudar para fget... colocar tudo na mesma linha
Consulta ler_Pedido_Consulta(){

	Consulta c;
	int n;

    FILE *file;
    file = fopen("PedidoConsulta.txt", "r");

    if(file == NULL){
		perror("Erro de leitura do Ficheiro");
		exit(1);
    }   

    fscanf(file, "%d", &c.tipo);
	fscanf(file, "%s", c.descricao);
	fscanf(file, "%d", &c.pid_consulta);
	
    fclose(file);

	//c.tipo = n;    
    return c;
}


//Ver se a lista de consultas tem vaga para a consulta
int verificar_vaga_lista_consultas(){
	indice_lista_consultas = malloc(sizeof(int));
	
    for(int i = 0; i < 10; i++){
        if((lista_consultas[i]).tipo == -1){
            *indice_lista_consultas = i;
			return 1;
        }
    }
    return 0;
}

//Escolher qual dos contadores incrementar, dependendo do tipo de consulta
void incrementar_Contador_Consultas(int tipo){
	switch(tipo){
	case 1: count_consulta_1++; break;
	case 2: count_consulta_2++; break;
	case 3: count_consulta_3++; break;
	default: perror("Erro!"); break;
	}
}

//After Receiving the signal SIGALRM
void sinal_alarme(int sinal){
    if(sinal == SIGALRM){
        n_Alarme = 1;
    }
}

void tratar_Consulta(){

    int n = fork();

    if(n == 0){
        //Processo filho
        printf("Filhoooo\n");
		//Enviar SIGHUP a cliente
		kill(lista_consultas[*indice_lista_consultas].pid_consulta, SIGHUP);
		
		//Wait 10 seconds until end of consulta
		signal(SIGALRM, sinal_alarme);
        alarm(10);

        while(n_Alarme == 0){
            pause();    
        }

		printf("Consulta terminada na sala %d\n", *indice_lista_consultas);		

		//Send signal SIGTERM to cliente
		kill(lista_consultas[*indice_lista_consultas].pid_consulta, SIGTERM);
		exit(0);
    }

    //Processo Pai a espera que processo filho termine
    wait(NULL);

	lista_consultas[*indice_lista_consultas].tipo = -1;
    *indice_lista_consultas = *indice_lista_consultas - 1;
}


//After receiving signal SIGUSR1
void tratar_Pedido_consulta(){
    Consulta c = ler_Pedido_Consulta();

    printf("->Sinal Recebido!!\n");

    printf("Chegou novo pedido de consulta do tipo: %d, descricao: %s e PID: %d\n", c.tipo, c.descricao, c.pid_consulta);

    if(verificar_vaga_lista_consultas()){
		//como tem vaga na lista, adicionar a consulta a primeira posicao vaga
		lista_consultas[*indice_lista_consultas] = c;
		printf("adicionado!!\n");
	
		//Incrementar o contador do respetivo tipo da consulta
		incrementar_Contador_Consultas(c.tipo);

		//Fazer fork() e tratar a consulta
		tratar_Consulta();
	}
	//s3.3: if lista_consultas is full send signal sigusr2 to cliente 
    else{
        printf("Lista de consultas cheia\n");
        kill(c.pid_consulta, SIGUSR2);
		count_consulta_perdida++;
	}

    n = 1;
}



int main(){

	//Consulta *lista_consultas=NULL;

	//s1
	lista_consultas = malloc(10 * sizeof(int));
	for(int i = 0; i < 6; i++){
		lista_consultas[i].tipo = -1;
	}

	//s2: Create file SrvConsultas
	escrever_Pid_SrvConsultas();
	printf("%d\n", getpid());

	//s3: Tratar sinal SIGUSR1
    signal(SIGUSR1, tratar_Pedido_consulta);

	//Waiting to receive a signal

    while(n == 0){
        pause();
    }


}
