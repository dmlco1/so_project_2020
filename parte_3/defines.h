#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>
#include <sys/shm.h>
#include<unistd.h>
#define exit_on_null(s,m) if (s==NULL) { perror(m); exit(1); }
#define exit_on_error(s,m) if (s<0) { perror(m); exit(1);}
#define IPCS_KEY 0x0a92697

typedef struct{
	long tipo;
	struct{
		int tipo; //Tipo Consulta: 1-Normal, 2-COVID19, 3-Urgente
		char descricao[100]; //Descricao da consulta
		int pid_consulta; //PID do processoque quer fazer consulta
		int status; //Estado consulta: 1-Pedido, 2-Iniciada, 3-Terminada, 4-Recusada, 5-Cancelada
	}Consulta; //struct da consulta
}Mensagem; //Struct da mensagem a enviar