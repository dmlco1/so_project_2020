#include "defines.h"

#define DURACAO 10
#define TAMANHO 10

Consulta c;
Consulta *lista_consultas;
int *indice_lista_consultas = NULL;
int countTipo1;
int /*countTipo1,*/ countTipo2, countTipo3, countPerdidas;
int iniciar/*, acabar*/;
int id, status, msg_queue_id, msg_queue_status;
//void encerrar();



void encerrar(){
	printf("\nTipo 1: %d\nTipo 2: %d\nTipo 3: %d\nPerdidas: %d\n", countTipo1, countTipo2, countTipo3, countPerdidas);
	exit(1);
}

void main(){	
	indice_lista_consultas = malloc(sizeof(int));

	//Check if shared memory already exists
	if(shmget(IPCS_KEY, TAMANHO * sizeof(Consulta) + 5*sizeof(int), IPC_CREAT | IPC_EXCL | 0600) > 0){
		iniciar = 1;
	}

	//Ligar a shared memory
	id = shmget(IPCS_KEY, TAMANHO * sizeof(Consulta) + 5*sizeof(int), IPC_CREAT| 0600);
    Consulta *lista_consultas = (Consulta*)shmat( id, 0, 0 ); exit_on_null (lista_consultas, "Attach");
	
	//int *countTipo1 = (int*)shmat(id, 0, 0); exit_on_null(countTipo1, "Attach");
	
	//printf("INDICE INDICE %d", *indice_lista_consultas);
	
	//int indice_lista_consultas = (int*)((void*)lista_consultas + 10 * sizeof(Consulta));

	//printf("INDICE INDICE %d", indice_lista_consultas);
	//printf("INDICE VALOR %d", *indice_lista_consultas);

	//Se e a primeira vez a ser criada, inicializamos o array
	if(iniciar){
		for(int i = 0; i < TAMANHO; i++){
			lista_consultas[i].Dados_Consulta.tipo = -1;
	     }
		countTipo1 = 0; countTipo2 = 0; countTipo3 = 0; countPerdidas = 0;//colocar os counts a 0
		printf("Shared memory iniciada!\n");
	}
	
	//Ligar a message queue	
	msg_queue_id = msgget(IPCS_KEY, IPC_CREAT | 0600); 
	exit_on_error( msg_queue_id, "Erro a criar message queue");
	
	signal(SIGINT, encerrar);
	printf("PID MAIN %d\n", getpid()); 	
	
	//Ficar a espera de mensagens
	while(1){
		//s4: Tratar sinal SIGINT
//		signal(SIGINT, encerrar);

		//Esperar que receba mensagem do tipo 1
        msg_queue_status = msgrcv( msg_queue_id, &c, sizeof(c), 1, 0);
        exit_on_error (msg_queue_status, "Recepção");

		if(c.Dados_Consulta.status == 1){

			printf ("Chegou um novo pedido de consulta do tipo <%d>, descricao <%s> e PID <%d>\n", c.Dados_Consulta.tipo, c.Dados_Consulta.descricao, c.Dados_Consulta.pid_consulta);
					
	
			//fork -> Criar Servidor dedicado
			int n = fork();
			if(n == 0){	
				printf("PID FILHO %d\n", getpid());
				int vaga;
				for(int i = 0; i < TAMANHO; i++){
					if((lista_consultas[i]).Dados_Consulta.tipo == -1){
						*indice_lista_consultas = i;
						vaga = 1;
					}
			    }
				//Se tem vaga coloca a consulta no indice - Comeca no fim e vai para o inicio!
				if(vaga){
					lista_consultas[*indice_lista_consultas] = c;
					printf("Consulta agendada para a sala <%d>\n", *indice_lista_consultas);
					
					//incrementar o respetivo contador
					switch(c.Dados_Consulta.tipo){
					case 1: countTipo1++;printf("Incrementou o tipo para %d\n", countTipo1);break;
					case 2: countTipo2++;break;
					case 3: countTipo3++;break;
					default: perror("ERRO"); exit(0);
					}
					c.tipo = c.Dados_Consulta.pid_consulta;
					c.Dados_Consulta.status = 2;
                
					//Mandar mensagem tipo 2-Iniciada para o cliente
					msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
					exit_on_error(msg_queue_status, "Erro de envio");
					sleep(DURACAO);
			
					printf("Consulta terminada na sala <%d>\n", *indice_lista_consultas);
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
					countPerdidas++;
					c.tipo = c.Dados_Consulta.pid_consulta;
					c.Dados_Consulta.status = 4;
				
					//Mandar mensagem tipo 4-Perdida para o cliente
					msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
					exit_on_error(msg_queue_status, "Erro de envio");
					exit(0);				
				}
			}		
			printf("O pai esta ativo\n");
			//continue;
			printf("PID PAI %d\n", getpid()); 
		}
	}
}






