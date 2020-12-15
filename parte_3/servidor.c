#include "defines.h"

#define DURACAO 10
#define TAMANHO 10

//Struct do semaforo
struct sembuf UP = {0, 1, 0};
struct sembuf DOWN = {0, -1, 0};

Consulta *lista_consultas;
int *indice_lista_consultas, *countTipo1, *countTipo2, *countTipo3, *countPerdidas;
int iniciar, cancelar, receber, id, status, msg_queue_id, msg_queue_status, sem_id, sem_status;
void encerrar();
int childPid;


void cancelar_consulta(){
	cancelar = 1;
}

void update_count(Consulta c){
	//Zona de exclusao
	sem_status = semop(sem_id, &DOWN, 1);
    exit_on_error(status, "DOWN");
	switch(c.Dados_Consulta.tipo){
		case 1: *countTipo1 = *countTipo1 +1;break;
        case 2: *countTipo2 = *countTipo2 + 1;break;
        case 3: *countTipo3 = *countTipo3 + 1;break;
        default: perror("ERRO"); exit(0);
    }
	sem_status = semop(sem_id, &UP, 1);
    exit_on_error(status, "UP");
}

void main(){	
	Consulta c;
	
	//Check if shared memory already exists
	if(shmget(IPCS_KEY, TAMANHO * sizeof(Consulta) + 5*sizeof(int), IPC_CREAT | IPC_EXCL | 0600) > 0){
		iniciar = 1;
	}

	//Ligar a shared memory
	id = shmget(IPCS_KEY, TAMANHO * sizeof(Consulta) + 5*sizeof(int), IPC_CREAT| 0600);
    Consulta *lista_consultas = (Consulta*)shmat( id, 0, 0 ); exit_on_null (lista_consultas, "Attach");


	//Criar/ligar semaforo
	sem_id = semget(IPCS_KEY, 1, IPC_CREAT | 0600);
	exit_on_error(id, "Erro a criar/ligar semaforo");
	printf("Sem ID %d", sem_id);

	sem_status = semctl(sem_id, 0, SETVAL, 1);
	exit_on_error(status, "Erro a incicializar semaforo");

	indice_lista_consultas = (int*)((void*)lista_consultas + TAMANHO * sizeof(Consulta));
	countTipo1 = indice_lista_consultas + 1;	
	countTipo2 = countTipo1 + 1;
	countTipo3 = countTipo2 + 1;
	countPerdidas = countTipo3 + 1;

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
	msg_queue_id = msgget(IPCS_KEY, IPC_CREAT | 0600); 
	exit_on_error( msg_queue_id, "Erro a criar message queue");

	//signal(SIGINT, encerrar);
	signal(SIGINT, encerrar);

	//Ficar a espera de mensagens
	while(1){
		//Esperar que receba mensagem do tipo 1
        msg_queue_status = msgrcv(msg_queue_id, &c, sizeof(Consulta), 1, 0);
        exit_on_error(msg_queue_status, "Recepção");
		
		if(c.Dados_Consulta.status == 1){
			printf ("Chegou um novo pedido de consulta do tipo <%d>, descricao <%s> e PID <%d>\n", c.Dados_Consulta.tipo, c.Dados_Consulta.descricao, c.Dados_Consulta.pid_consulta);
			receber = 1;
		}			
		//fork -> Criar Servidor dedicado
		int n = fork();
		if(n == 0 && receber){
			childPid = getpid();	
			int vaga, sala;
			
			//Zona de exclusao -> Impedir que 2 consultas tentem aceder a mesma posicao da lista de consultas
			sem_status = semop(sem_id, &DOWN, 1);
            exit_on_error(status, "DOWN");
			for(int i = 0; i < TAMANHO; i++){
				
			if((lista_consultas[i]).Dados_Consulta.tipo == -1){
					*indice_lista_consultas = i;
					sala = i;
					vaga = 1;
					break;
				}			
			}
		
			//Se tem vaga coloca a consulta no indice - Comeca no fim e vai para o inicio!
			if(vaga){
				//Zona de exclusao
				sem_status = semop(sem_id, &DOWN, 1);
				exit_on_error(status, "DOWN");
				printf("SEM Status %d\n", sem_status);
				//Colocar consulta recebida na lista
				lista_consultas[*indice_lista_consultas] = c;
				sem_status = semop(sem_id, &UP, 1);
				exit_on_error(status, "UP");
				printf("status %d\n", sem_status);			
				printf("Consulta agendada para a sala <%d>\n", sala);
				
				//incrementar o respetivo contador
				c.tipo = c.Dados_Consulta.pid_consulta;
				c.Dados_Consulta.status = 2;
				//Mandar mensagem tipo 2-Iniciada para o cliente
				msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
				exit_on_error(msg_queue_status, "Erro de envio");
				
				signal(SIGALRM, cancelar_consulta);
				
				//Esperar a duracao da consulta
				alarm(DURACAO);
				//Enquanto a consulta esta a ser processada, verificar se e recebida uma mensagem de cancelamento
				while(cancelar == 0){
					msg_queue_status = msgrcv(msg_queue_id, &c, sizeof(Consulta), c.Dados_Consulta.pid_consulta, IPC_NOWAIT);
					if ( msg_queue_status == -1 ) {
						continue;
					}
					//Se o servidor dedicado receber uma mensagem com status 5 -> cancelar consulta
					if(c.Dados_Consulta.status == 5){
						printf("\nConsulta cancelada pelo utilizador %d\n", c.Dados_Consulta.pid_consulta); 
						lista_consultas[sala].Dados_Consulta.tipo = -1;
						exit(0);
					}
				}
				
				//Apos a consulta ser concluida
				printf("Consulta terminada na sala <%d>\n", sala);
				c.tipo = c.Dados_Consulta.pid_consulta;
				c.Dados_Consulta.status = 3;
				update_count(c);					
			
				//Mandar mensagem tipo 3-Terminada para o cliente
				msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
				exit_on_error(msg_queue_status, "Erro de envio");
			
				//Depois de terminar a consulta, retira-la da lista
                lista_consultas[sala].Dados_Consulta.tipo = -1;
				exit(0);
			}
			else{
				//Zona de exclusao
                sem_status = semop(sem_id, &DOWN, 1);
                exit_on_error(status, "DOWN");
				//Se nao houver vagas na lista
				printf("Lista de consultas cheia\n");
				*countPerdidas = *countPerdidas + 1;
				//Atualizar semaforo
				sem_status = semop(sem_id, &UP, 1);
                exit_on_error(status, "UP");
				
				c.tipo = c.Dados_Consulta.pid_consulta;
				c.Dados_Consulta.status = 4;
				
				//Mandar mensagem tipo 4-Perdida para o cliente
				msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
				exit_on_error(msg_queue_status, "Erro de envio");
				exit(0);				
			}
		}
		waitpid(childPid, NULL, IPC_NOWAIT);
	}
}


void encerrar(){
    printf("\nTipo 1: %d\n", *countTipo1);
    printf("Tipo 2: %d\n", *countTipo2);
    printf("Tipo 3: %d\n", *countTipo3);
    printf("Perdidas: %d\n", *countPerdidas);
    exit(0);
}


