#include "defines.h"

#define DURACAO 10
#define TAMANHO 10

Consulta *lista_consultas=NULL;

void main(){
	Consulta c;
	
	int id, status;
	
	//Check if shared memory already exists
	if (shmget(IPCS_KEY, TAMANHO * sizeof(Consulta), IPC_CREAT| IPC_EXCL | 0600) > 0){
		id = shmget(IPCS_KEY, TAMANHO * sizeof(Consulta), IPC_CREAT| IPC_EXCL | 0600);
		exit_on_error(id, "Erro a criar shared memory");
		
		//exists 
		printf("ExisShared memory iniciada!");
	}
	id = shmget(IPCS_KEY, TAMANHO * sizeof(Consulta), IPC_CREAT| 0600);
	exit_on_error(id, "Erro a criar shared memory");

	
}
