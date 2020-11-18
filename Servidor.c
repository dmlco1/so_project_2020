#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include "consulta.h"

int count_consulta_1;
int count_consulta_2;
int count_consulta_3;
int count_consulta_perdida;

int n;

Consulta *lista_consultas=NULL;

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


int verificar_vaga_lista_consultas(){

    for(int i = 0; i < 10; i++){
        if((lista_consultas[i]).tipo == -1){
            return 1;
        }
    }
    return 0;
}


void tratar_Pedido_consulta(){
    Consulta c = ler_Pedido_Consulta();

    printf("->Sinal Recebido!!\n");

    printf("Chegou novo pedido de consulta do tipo: %d, descricao: %s e PID: %d\n", c.tipo, c.descricao, c.pid_consulta);

    if(verificar_vaga_lista_consultas()){
        for(int i = 0; i < 10; i++){
            if((lista_consultas[i]).tipo == -1){
                lista_consultas[i] = c;
                printf("%d\n", lista_consultas[i].tipo);
                printf("%s\n", lista_consultas[i].descricao);
                printf("%d\n", lista_consultas[i].pid_consulta);
			}
        }
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
	for(int i = 0; i < 10; i++){
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
