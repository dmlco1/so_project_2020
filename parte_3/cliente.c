#include "defines.h"

void main(){

	Mensagem m;
	int msg_queue_id, msg_queue_status;
	
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




}
