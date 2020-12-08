#include "defines.h"

Mensagem m;
int msg_queue_id, msg_queue_status, acabar, iniciar;

//void cancelar_espera(Mensagem msg);

void cancelar_espera(){
    printf("Paciente cancelou o pedido\n");
	
	//ligar a message queue do servidor dedicado
    msg_queue_id = msgget ( IPCS_KEY, 0600 | IPC_CREAT ); 
    exit_on_error (msg_queue_id, "Erro a ligar a mssage queue");
	
	m.Consulta.status = 5;
	
	msg_queue_status = msgsnd(msg_queue_id, &m, sizeof(m), 0);
    exit_on_error (msg_queue_status, "Recepção");

    exit(0);
}

void main(){

	char tipo[10];

	// c1) c2) Introduzir os dados de consulta
	printf("Tipo de Consulta: ");
	fgets(tipo, 10, stdin);
	m.Consulta.tipo = atoi(tipo);
	//Confirmar se o input e valido
	if(m.Consulta.tipo != 1 && m.Consulta.tipo != 2 && m.Consulta.tipo != 3){printf("Input invalido\n");exit(1);}

	printf("Descricao: ");
    fgets(m.Consulta.descricao, 100, stdin);
    m.Consulta.descricao[strlen(m.Consulta.descricao)-1] = 0;

	m.Consulta.pid_consulta = getpid();
	m.Consulta.status = 1;
	m.tipo = 1;

	//ligar a message queue
	msg_queue_id = msgget ( IPCS_KEY, 0600 | IPC_CREAT ); 
	exit_on_error (msg_queue_id, "Erro a ligar a mssage queue");

	//Enviar mensagem com o tipo 1
	msg_queue_status = msgsnd(msg_queue_id, &m, sizeof(m), 0); 
	exit_on_error(msg_queue_status, "Erro de envio");

	//c7)
    signal(SIGINT, cancelar_espera);

	while(acabar == 0){
		//Receber mensagem
		msg_queue_status = msgrcv(msg_queue_id, &m, sizeof(m), getpid(), 0);
		exit_on_error (msg_queue_status, "Recepção");
	
		//c4) c5) c6)	
		switch(m.Consulta.status){
        case 2: iniciar = 1;printf("Consulta iniciada para o processo %d\n", m.Consulta.pid_consulta);break;
        case 3: (iniciar) ? printf("Consulta terminada para o processo %d\n", m.Consulta.pid_consulta) : perror("Erro");exit(0);break;
		case 4: printf("Consulta nao e possivel para o processo %d\n", m.Consulta.pid_consulta);exit(0);break;
        default: perror("ERRO"); exit(0);
        }

	}

}

/*
void cancelar_espera(Mensagem msg){
    printf("Paciente cancelou o pedido\n");

    //ligar a message queue do servidor dedicado
    msg_queue_id = msgget ( IPCS_KEY, 0600 | IPC_CREAT );
    exit_on_error (msg_queue_id, "Erro a ligar a mssage queue");

    msg.Consulta.status = 5;

    msg_queue_status = msgsnd(msg_queue_id, &m, sizeof(m), 0);
    exit_on_error (msg_queue_status, "Recepção");

    exit(0);
}
*/



