#include "defines.h"

#define DURACAO 10
#define TAMANHO 10

Mensagem m;
Mensagem *lista_consultas;
int *indice_lista_consultas = NULL;

int countTipo1 = 0;
int countTipo2, countTipo3, countPerdidas;
int iniciar, acabar;
int id, status, msg_queue_id, msg_queue_status;

void verificar_vaga();

void main(){	
	indice_lista_consultas = malloc(sizeof(int));
	//Check if shared memory already exists
	if (shmget(IPCS_KEY, TAMANHO * sizeof(Mensagem), IPC_CREAT | IPC_EXCL | 0600) > 0){
		iniciar = 1;
	}

	//Ligar a shared memory
	id = shmget(IPCS_KEY, TAMANHO * sizeof(Mensagem), IPC_CREAT| 0600);
    Mensagem *lista_consultas = (Mensagem*)shmat ( id, 0, 0 ); exit_on_null (lista_consultas, "Attach");
	
	//Se e a primeira vez a ser criada, inicializamos o array
	if(iniciar){
		for(int i = 0; i < TAMANHO; i++){
			lista_consultas[i].Consulta.tipo = -1;
	     }
		countTipo1 = 0; countTipo2 = 0; countTipo3 = 0; //colocar os counts a 0
		printf("Shared memory iniciada!\n");
	}
	
	//Ligar a message queue	
	msg_queue_id = msgget(IPCS_KEY, IPC_CREAT | 0600); 
	exit_on_error( msg_queue_id, "Erro a criar message queue");
	
	//Ficar a espera de mensagens
	while(acabar == 0){
		//Esperar que receba mensagem do tipo 1
        msg_queue_status = msgrcv( msg_queue_id, &m, sizeof(m), 1, 0);
        exit_on_error (msg_queue_status, "Recepção");

        printf ("Chegou um novo pedido de consulta do tipo <%d>, descricao <%s> e PID <%d>\n", m.Consulta.tipo, m.Consulta.descricao, m.Consulta.pid_consulta);
		
		//fork
		int n = fork();
		if(n == 0){			
			int vaga;
			for(int i = 0; i < TAMANHO; i++){
				if((lista_consultas[i]).Consulta.tipo == -1){
					*indice_lista_consultas = i;
					vaga = 1;
				}
            }
			//Se tem vaga coloca a consulta no indice - Comeca no fim e vai para o inicio!
			if(vaga){
				lista_consultas[*indice_lista_consultas] = m;
                printf("Consulta agendada para a sala <%d>\n", *indice_lista_consultas);
				
				//incrementar o respetivo contador
				switch(m.Consulta.tipo){
				case 1: countTipo1++;break;
				case 2: countTipo2++;break;
				case 3: countTipo3++;break;
				default: perror("ERRO"); exit(0);
				}
				
				m.tipo = m.Consulta.pid_consulta;
                m.Consulta.status = 2;
                
                //Mandar mensagem tipo 2-Iniciada para o cliente
                msg_queue_status = msgsnd(msg_queue_id, &m, sizeof(m), 0);
                exit_on_error(msg_queue_status, "Erro de envio");
								

				sleep(DURACAO);
			
				printf("Consulta terminada na sala <%d>\n", *indice_lista_consultas);
				m.tipo = m.Consulta.pid_consulta;
                m.Consulta.status = 3;

                //Mandar mensagem tipo 3-Terminada para o cliente
                msg_queue_status = msgsnd(msg_queue_id, &m, sizeof(m), 0);
                exit_on_error(msg_queue_status, "Erro de envio");

				exit(0);
			}
			else{
				//Se nao houver vagas na lista
				printf("Lista de consultas cheia\n");
				countPerdidas++;
				m.tipo = m.Consulta.pid_consulta;
				m.Consulta.status = 4;
				
				//Mandar mensagem tipo 4-Perdida para o cliente
				msg_queue_status = msgsnd(msg_queue_id, &m, sizeof(m), 0);
				exit_on_error(msg_queue_status, "Erro de envio");
				exit(0);				
			}
		}		
		printf("O pai esta ativo\n");
	}

}









