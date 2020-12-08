#include "defines.h"

#define DURACAO 10
#define TAMANHO 10
Mensagem m;
Mensagem *lista_consultas;

int *indice_lista_consultas = NULL;
int countTipo1, countTipo2, countTipo3, countPerdidas;
int acabar;
int id, status, msg_queue_id, msg_queue_status;

int tem_vaga(){
	printf("entrou\n");
    for(int i = 0; i < TAMANHO; i++){
        printf("AAAA%d\n", i);
		printf("TIPOOOO %d\n", lista_consultas[i].Consulta.tipo);
		if((lista_consultas[i]).Consulta.tipo == -1){
			*indice_lista_consultas = i;
            printf("YES\n");
			return 1;
			
        }
		printf("NOP\n");
    }
    return 0;
}

void tratar_consulta(){
	int n = fork();

	if(n == 0){
		printf("FILHO\n");
		if(tem_vaga() == 0){
			printf("Lista de consultas cheia\n");
			//Enviar mensagem 4-Recusada
			m.Consulta.status = 4;
			m.tipo = m.Consulta.pid_consulta;
			countPerdidas++;
			msg_queue_status = msgsnd(msg_queue_id, &m, sizeof(m), 0);
			exit_on_error(msg_queue_status, "Erro de envio");
		}
	}
	wait();
	printf("PAI\n");
}

void mensagem_recebida(){
	//Esperar que receba mensagem do tipo 1
        msg_queue_status = msgrcv( msg_queue_id, &m, sizeof(m), 1, 0);
        exit_on_error (msg_queue_status, "Recepção");

        printf ("Chegou um novo pedido de consulta do tipo <%d>, descricao <%s> e PID <%d>\n", m.Consulta.tipo, m.Consulta.descricao, m.Consulta.pid_consulta);
		printf("TIPOOOO %d\n", lista_consultas[1].Consulta.tipo);
	//tratar_consulta();
}

void main(){
	//Mensagem *lista_consultas = shmat(getpid(), 0 ,0);
	//int id, status, msg_queue_id, msg_queue_status;
	//lista_consultas = malloc(TAMANHO * sizeof(Mensagem));

	//Check if shared memory already exists
	if (shmget(IPCS_KEY, TAMANHO * sizeof(Mensagem), IPC_CREAT | IPC_EXCL | 0600) > 0){
		id = shmget(IPCS_KEY, TAMANHO * sizeof(Mensagem), IPC_CREAT| 0600);
	
	//s1
		Mensagem *lista_consultas = shmat(id, 0, 0);
		exit_on_null (lista_consultas, "Attach");
		
		countTipo1 = 0; countTipo2 = 0; countTipo3 = 0; countPerdidas = 0;

		//limpar a lista
		lista_consultas = malloc(TAMANHO * sizeof(Mensagem));
		for(int i = 0; i < TAMANHO; i++){
			lista_consultas[i].Consulta.tipo = 1;
			printf("TIPOOOO %d\n", lista_consultas[i].Consulta.tipo);
		}
	
		printf("Shared memory iniciada!\n");
	}
	else{
		id = shmget(IPCS_KEY, TAMANHO * sizeof(m.Consulta), IPC_CREAT| 0600);
		exit_on_error(id, "PPP Erro a criar shared memory");
		Mensagem *lista_consultas = shmat(id, 0, 0);
		exit_on_null (lista_consultas, "Attach");
	}
	//Ligar a message queue
	msg_queue_id = msgget( IPCS_KEY, IPC_CREAT | 0600 ); 
	exit_on_error( msg_queue_id, "Erro a criar message queue");

	printf("Ttt %d\n", lista_consultas[1].Consulta.tipo);

	//Ficar a espera de mensagens
	while(acabar == 0){
		mensagem_recebida();

		//pause();
	}

}





