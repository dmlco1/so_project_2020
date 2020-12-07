#include "defines.h"

#define DURACAO 10
#define TAMANHO 10

Mensagem m;
m.Consulta lista[10]; 
//lista = malloc(TAMANHO*sizeof(m.Consulta));
//lista_consultas = malloc(TAMANHO * sizeof(int));


int countTipo1, countTipo2, countTipo3, countPerdidas;

void main(){
	//Mensagem m;
	//m.Consulta *lista=NULL;
	//lista = malloc(TAMANHO * sizeof(m.Consulta));

	int id, status;
	
	//Check if shared memory already exists
	if (shmget(IPCS_KEY, TAMANHO * sizeof(Mensagem), IPC_CREAT | IPC_EXCL | 0600) > 0){
		id = shmget(IPCS_KEY, TAMANHO * sizeof(Mensagem), IPC_CREAT| 0600);
		
	//s1
    
	//lista_consultas = malloc(TAMANHO * sizeof(int));
    /*for(int i = 0; i < TAMANHO; i++){
        lista_consultas[i].tipo = -1;
    }
	*/
	countTipo1 = 0; countTipo2 = 0; countTipo3 = 0; countPerdidas = 0;
	
	printf("Shared memory iniciada! e count: %d\n", countTipo1);
	}
	else{
		id = shmget(IPCS_KEY, TAMANHO * sizeof(m.Consulta), IPC_CREAT| 0600);
		exit_on_error(id, "PPP Erro a criar shared memory");
		countTipo1 = 2;
		printf("Ja existe %d\n", countTipo1);
	}
}
