#include "defines.h"

#define DURACAO 10
#define TAMANHO 10

Consulta c;
Consulta *lista_consultas;
int *indice_lista_consultas;
int *countTipo1, *countTipo2,*countTipo3, *countPerdidas;
int iniciar, acabar;
//int *countTipo2;
int id, status, msg_queue_id, msg_queue_status;
void encerrar();

/*
void encerrar(){
	printf("count tipo 1 %d\n", *countTipo1);
	printf("\nTipo 1: %d\n", *countTipo1);
	printf("count tipo 3 %d\n", *countTipo3);
    printf("\nTipo 3: %d\n", *countTipo3);
	printf("count tipo P %d\n", *countPerdidas);
    printf("\nTipo P: %d\n", *countPerdidas);
	printf("count tipo 2 %d\n", *countTipo2);
    printf("\nTipo 2: %d\n", *countTipo2);
	//printf("count tipo P %d\n", *countPerdidas);
    //printf("\nTipo P: %d\n", *countPerdidas);
	exit(0);
}
*/

void update_count(){
	switch(c.Dados_Consulta.tipo){
		case 1: printf("TIPO 1\n");printf("dentro case esta %d\n", *countTipo1);*countTipo1 = *countTipo1 +1;printf("Incrementou o tipo 1 para %d\n", *countTipo1);break;
        case 2: printf("Tipo 2\n");printf("dentro case esta %d\n", *countTipo2);*countTipo2 = *countTipo2 + 1;printf("Incrementou o tipo 2 para %d\n", *countTipo2);break;
        case 3: printf("TP 3\n");*countTipo3 = *countTipo3 + 1;printf("Incrementou o tipo 3 para %d\n", countTipo3);break;
        default: perror("ERRO"); exit(0);
    }
}

void main(){	
	//indice_lista_consultas = malloc(sizeof(int));

	//Check if shared memory already exists
	if(shmget(IPCS_KEY, TAMANHO * sizeof(Consulta) + 5*sizeof(int), IPC_CREAT | IPC_EXCL | 0600) > 0){
		iniciar = 1;
	}

	//Ligar a shared memory
	id = shmget(IPCS_KEY, TAMANHO * sizeof(Consulta) + 5*sizeof(int), IPC_CREAT| 0600);
    Consulta *lista_consultas = (Consulta*)shmat( id, 0, 0 ); exit_on_null (lista_consultas, "Attach");

	indice_lista_consultas = (int*)((void*)lista_consultas + TAMANHO * sizeof(Consulta));

	countTipo1 = indice_lista_consultas + 1;	
	printf("count 1 %d\n", countTipo1);
	printf("count tipo 1 %d\n", *countTipo1);

	countTipo2 = countTipo1 + 1;
	printf("count 2 sera %d\n", countTipo2);
	printf("count tipo 2 %d\n", *countTipo2);
	
	countTipo3 = countTipo2 + 1;
	printf("count 3 %d\n", countTipo3);
    printf("count tipo 3 %d\n", *countTipo3);

	countPerdidas = countTipo3 + 1;
    printf("count P %d\n", countPerdidas);
    printf("count tipo P %d\n", *countPerdidas);

	//Se e a primeira vez a ser criada, inicializamos o array
	if(iniciar){
		for(int i = 0; i < TAMANHO; i++){
			lista_consultas[i].Dados_Consulta.tipo = -1;
	     }
		*countTipo1 = 0; 
		*countTipo2 = 0; 
		*countTipo3 = 0; 
		*countPerdidas = 0;//colocar os counts a 0
		printf("Shared memory iniciada!\n");
	}
	
	//Ligar a message queue	
	msg_queue_id = msgget(IPCS_KEY, IPC_CREAT | 0666); 
	exit_on_error( msg_queue_id, "Erro a criar message queue");

	
	//signal(SIGINT, encerrar);
	signal(SIGINT, encerrar);

	//Ficar a espera de mensagens
	while(1){

		//Consulta con;
		printf("1 antes msgrcv %d\n", countTipo1);
        printf("2 antes msgrcv %d\n", countTipo2);
		printf("1 antes msgrcv %d\n", *countTipo1);
        printf("2 antes msgrcv %d\n", *countTipo2);
	
		//Esperar que receba mensagem do tipo 1
        msg_queue_status = msgrcv(msg_queue_id, &c, sizeof(Consulta), 1, 0);
        exit_on_error(msg_queue_status, "Recepção");
		printf("depois msgrcv esta a %d\n", *countTipo1);
        printf("dentro msgrcv esta a %d\n", *countTipo2);


		if(c.Dados_Consulta.status == 1){
			printf ("Chegou um novo pedido de consulta do tipo <%d>, descricao <%s> e PID <%d>\n", c.Dados_Consulta.tipo, c.Dados_Consulta.descricao, c.Dados_Consulta.pid_consulta);
					
			//fork -> Criar Servidor dedicado

			int n = fork();
			if(n == 0){	
				int vaga;
				for(int i = 0; i < TAMANHO; i++){
					if((lista_consultas[i]).Dados_Consulta.tipo == -1){
						*indice_lista_consultas = i;
						vaga = 1;
						break;
					}
			    }
				//Se tem vaga coloca a consulta no indice - Comeca no fim e vai para o inicio!
				if(vaga){
					lista_consultas[*indice_lista_consultas] = c;
					
					printf("Consulta agendada para a sala <%d>\n", *indice_lista_consultas);
					update_count();

					//incrementar o respetivo contador
					c.tipo = c.Dados_Consulta.pid_consulta;
					c.Dados_Consulta.status = 2;
					printf("Enviar a cliente\n") ;               
					//Mandar mensagem tipo 2-Iniciada para o cliente
					msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
					exit_on_error(msg_queue_status, "Erro de envio");
					
					if(c.Dados_Consulta.status == 5){printf("RECEBIIIII");exit(0);}
					//alarm(DURACAO);
					printf("SLEEP\n");
					sleep(DURACAO);
			
					printf("Consulta terminada na sala <%d>\n", indice_lista_consultas);
					c.tipo = c.Dados_Consulta.pid_consulta;
					c.Dados_Consulta.status = 3;

					//Mandar mensagem tipo 3-Terminada para o cliente
					msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
					exit_on_error(msg_queue_status, "Erro de envio");
					exit(0);
				}
				else{
					//Se nao houver vagas na lista
					printf("Lista de consultas cheia\n");
					*countPerdidas = *countPerdidas + 1;
					c.tipo = c.Dados_Consulta.pid_consulta;
					c.Dados_Consulta.status = 4;
				
					//Mandar mensagem tipo 4-Perdida para o cliente
					msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
					exit_on_error(msg_queue_status, "Erro de envio");
					exit(0);				
				}
			}		
			wait(NULL);
			printf("O pai esta ativo\n");
		}
	}
}


void encerrar(){
    printf("count tipo 1 %d\n", *countTipo1);
    //print(msg_queue_id, &c, sizeof(c), 0);
    printf("count tipo 3 %d\n", *countTipo3);
    printf("\nTipo P: %d\n", *countPerdidas);
    printf("\nTipo 2: %d\n", *countTipo2);
    //printf("count tipo P %d\n", *countPerdidas);
    //printf("\nTipo P: %d\n", *countPerdidas);
    exit(0);
}



