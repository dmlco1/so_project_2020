#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "consulta.h"
#include<unistd.h>

typedef struct{
	int perdidas;
	int tipo1;
	int tipo2;
	int tipo3;
}Tipos;


typedef struct{
    int perdidas;
    int tipo1;
    int tipo2;
    int tipo3;
}TiposFile;

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
    char valor[100];

    FILE *file;
    file = fopen("PedidoConsulta.txt", "r");

    if(file == NULL){
		perror("Erro de leitura do Ficheiro\n");
		exit(1);
    }   

	fgets(linha, 100, file); 
	
    fclose(file);

	//Ler tipo consulta
	obter_campo(linha, valor, ',', 0);
	c.tipo = atoi(valor);	

	//Ler descricao consulta
	obter_campo(linha, c.descricao, ',', 1);

	//Ler pid consulta
	obter_campo(linha, valor, ',', 2);
    c.pid_consulta = atoi(valor); 
	
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

void atualizarStats(){
	//Check if file existis -> if not we just write the values in it
	if(access("StatsConsultas.dat", F_OK) == -1){
		
		FILE *file;
		Tipos t;
		file = fopen("StatsConsultas.dat", "w");
		
		//Colocar os valores da sessao no Tipo t
		t.perdidas = count_consulta_perdida;
        t.tipo1 = count_consulta_1;
        t.tipo2 = count_consulta_2;
        t.tipo3 = count_consulta_3;
		
		//Escrever no ficheiro o Tipo t, em que cada campo contem os valores da sessao
		fwrite(&t, sizeof(t), 1, file);

		fclose(file);
		exit(0);
	}
	//if file already exists we need to read all the values first and then update the stats
	else{
		FILE *file;
		
		Tipos t; //Tipos atualizados que irao ser colocados no ficheiro
		TiposFile tf; //Tipos lidos no ficheiro 

		file = fopen("StatsConsultas.dat", "r");
		fread(&tf, sizeof(tf), 1, file); //Colocar todos os valores do ficheiro no Tipo tf
		fclose(file);

		//Atualizar os dados e colocar os valores nos campos do Tipo t
		t.perdidas = tf.perdidas + count_consulta_perdida;
		t.tipo1 = tf.tipo1 + count_consulta_1;
		t.tipo2 = tf.tipo2 + count_consulta_2;
		t.tipo3 = tf.tipo3 + count_consulta_3;

		printf("O valor era %d\n", tf.tipo2);
		printf("O valor de 2 vai ser %d\n", t.tipo2);
	    //Escrever no ficheiro os dados atualizados (contidos no Tipo t)    
		file = fopen("StatsConsultas.dat", "w");
        fwrite(&t, sizeof(t), 1, file);
        fclose(file);
	}	
	
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
