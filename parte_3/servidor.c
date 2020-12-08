#include "defines.h"

#define DURACAO 10
#define TAMANHO 10

Mensagem m;
Mensagem *lista_consultas;

int countTipo1, countTipo2, countTipo3, countPerdidas;
int iniciar, acabar;
int id, status, msg_queue_id, msg_queue_status;

void main(){	
	//Check if shared memory already exists
	if (shmget(IPCS_KEY, TAMANHO * sizeof(Mensagem), IPC_CREAT | IPC_EXCL | 0600) > 0){
		iniciar = 1;
	}

	id = shmget(IPCS_KEY, TAMANHO * sizeof(Mensagem), IPC_CREAT| 0600);
    Mensagem *lista_consultas = (Mensagem*)shmat ( id, 0, 0 ); exit_on_null (lista_consultas, "Attach");
	
	//Se e a primeira vez a ser criada, inicializamos o array
	if(check()){
		printf("Como nao exsitia iniciamos\n");
		for(int i = 0; i < TAMANHO; i++){
			lista_consultas[i].Consulta.tipo = 1;
			printf("TIPOOOO %d\n", lista_consultas[i].Consulta.tipo);
	     }
		printf("Shared memory iniciada!\n");
	}
	
	//Ligar a message queue	
	msg_queue_id = msgget(IPCS_KEY, IPC_CREAT | 0600); 
	exit_on_error( msg_queue_id, "Erro a criar message queue");
	printf("AQUI\n");
	
	//Ficar a espera de mensagens
	while(acabar == 0){
		//Esperar que receba mensagem do tipo 1
        msg_queue_status = msgrcv( msg_queue_id, &m, sizeof(m), 1, 0);
        exit_on_error (msg_queue_status, "Recepção");

        printf ("Chegou um novo pedido de consulta do tipo <%d>, descricao <%s> e PID <%d>\n", m.Consulta.tipo, m.Consulta.descricao, m.Consulta.pid_consulta);
		
		
		
		//pause();
	}

}





