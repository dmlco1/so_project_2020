#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "consulta.h"
#include<unistd.h>

int count_consulta_1;
int count_consulta_2;
int count_consulta_3;
int count_consulta_perdida;

int n_Alarme;
int acabar = 0;


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


void obter_campo(char linha[], char total[], char delimitador, int indice){
	int k = 0;
    int j = 0;

    for(int i = 0; linha[i] != '\0'; i++){
        if(linha[i] == delimitador){
            k++;
        }
        else if(k == indice){
            total[j++] = linha[i];
        }
    }
    total[j] = '\0'; 
}


Consulta ler_Pedido_Consulta(){

	Consulta c;

	char linha[100];
    char lido[70];

    FILE *file;
    file = fopen("PedidoConsulta.txt", "r");

    if(file == NULL){
		perror("Erro de leitura do Ficheiro\n");
		exit(1);
    }   

	fgets(linha, 100, file); 
	
    fclose(file);

	//Ler tipo consulta
	obter_campo(linha, lido, ',', 0);
	c.tipo = atoi(lido);	

	//Ler descricao consulta
	obter_campo(linha, c.descricao, ',', 1);

	//Ler pid consulta
	obter_campo(linha, lido, ',', 2);
    c.pid_consulta = atoi(lido); 
	
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
    //*indice_lista_consultas = *indice_lista_consultas - 1;
}


//After receiving signal SIGUSR1
void tratar_Pedido_consulta(){
    Consulta c = ler_Pedido_Consulta();

    printf("Chegou novo pedido de consulta do tipo: %d, descricao: %s e PID: %d\n", c.tipo, c.descricao, c.pid_consulta);

    if(verificar_vaga_lista_consultas()){
		//como tem vaga na lista, adicionar a consulta a primeira posicao vaga
		lista_consultas[*indice_lista_consultas] = c;
		printf("Consulta agendada para para a sala %d\n", *indice_lista_consultas);
	
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
}


void update_stat(char linha[], char total[], char delimitador, int indice){
    int k = 0;
    int j = 0;

    for(int i = 0; linha[i] != '\0'; i++){
        if(linha[i] == delimitador){
            k++;
        }
        else if(k == indice){
            total[j++] = linha[i];
        }
    }
    total[j] = '\0';
}



void atualizarStats(){

	 FILE *file;

    file = fopen("StatsConsultas.dat", "w");

	//if the file already exists update values
	int count;
	char linha[100];
	char valor[100];
	
	//update consultas perdidas
	fgets(linha, 100, file);	

	update_stat(linha, valor, ':', 1);	
	count = atoi(valor) + count_consulta_perdida;
	fprintf(file, "Perdidos:%d\n", count);

	//update consultas tipo 1
	fgets(linha, 100, file);    

    update_stat(linha, valor, ':', 1);
    count = atoi(valor) + count_consulta_1;
    fprintf(file, "Tipo 1:%d\n", count);

	//update consultas tipo 2
    fgets(linha, 100, file);

    update_stat(linha, valor, ':', 1);
    count = atoi(valor) + count_consulta_2;
    fprintf(file, "Tipo 2:%d\n", count);

	//update consultas tipo 3
    fgets(linha, 100, file);
    
    update_stat(linha, valor, ':', 3);
    count = atoi(valor) + count_consulta_3;
    fprintf(file, "Tipo 3:%d\n", count);

	fclose(file);
}


void encerrar(){

    //remove file PedidoConsulta.txt
    remove("SrvConsultas.pid");
    printf("\nSrvConsultas.pid removido com sucesso!\n");
	atualizarStats();
    acabar = 1;
}


int main(){

	//s1
	lista_consultas = malloc(10 * sizeof(int));
	for(int i = 0; i < 10; i++){
		lista_consultas[i].tipo = -1;
	}

	//s2: Create file SrvConsultas
	escrever_Pid_SrvConsultas();
	printf("%d\n", getpid());

	//s3: Tratar sinal SIGUSR1
    signal(SIGUSR1, tratar_Pedido_consulta);

	//s4: Tratar sinal SIGINT
    signal(SIGINT, encerrar);

	//Waiting to receive a signal
    while(acabar==0){
        pause();
    }


}
