#include "defines.h"

Consulta c;
int msg_queue_id, msg_queue_status/*, acabar*/, iniciar;

//void cancelar_espera(Mensagem msg);
void cancelar_espera(){
    printf("\nPaciente cancelou o pedido\n");
	c.Dados_Consulta.status = 5;
	c.tipo = getpid();	
	
	msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0);
    exit_on_error (msg_queue_status, "Erro de envio");
    exit(0);
}

void main(){
	char tipo[10];

	// c1) c2) Introduzir os dados de consulta
	printf("Tipo de Consulta: ");
	fgets(tipo, 10, stdin);
	c.Dados_Consulta.tipo = atoi(tipo);
	
	//Confirmar se o input e valido
	if(c.Dados_Consulta.tipo != 1 && c.Dados_Consulta.tipo != 2 && c.Dados_Consulta.tipo != 3){printf("Input invalido\n");exit(1);}

	printf("Descricao: ");
    fgets(c.Dados_Consulta.descricao, 100, stdin);
    c.Dados_Consulta.descricao[strlen(c.Dados_Consulta.descricao)-1] = 0;

	c.Dados_Consulta.pid_consulta = getpid();
	c.Dados_Consulta.status = 1;
	c.tipo = 1;

	//ligar a message queue
	msg_queue_id = msgget ( IPCS_KEY, IPC_CREAT | 0600); 
	exit_on_error (msg_queue_id, "Erro a ligar a mssage queue");


	//Enviar mensagem com o tipo 1
	msg_queue_status = msgsnd(msg_queue_id, &c, sizeof(c), 0); 
	exit_on_error(msg_queue_status, "Erro de envio");

	printf("Enviada com tipo %d e status %d\n", c.Dados_Consulta.tipo, c.Dados_Consulta.status);

	//c7)
    signal(SIGINT, cancelar_espera);

	while(1){
		//Receber mensagem
		msg_queue_status = msgrcv(msg_queue_id, &c, sizeof(c), getpid(), 0);
		exit_on_error (msg_queue_status, "Recepção");
		
		//c4) c5) c6)	
		switch(c.Dados_Consulta.status){
        case 2: iniciar = 1;printf("Consulta iniciada para o processo %d\n", c.Dados_Consulta.pid_consulta);break;
        case 3: (iniciar) ? printf("Consulta concluida para o processo %d\n", c.Dados_Consulta.pid_consulta) : perror("Erro");exit(0);break;
		case 4: printf("Consulta nao e possivel para o processo %d\n", c.Dados_Consulta.pid_consulta);exit(0);break;
        default: perror("ERRO"); exit(0);
        }
	}
}

