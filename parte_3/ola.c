#include "defines.h"

#define DURACAO 10
#define TAMANHO 10

void main(){
    Consulta c;

    int id, status;
    id = shmget(IPCS_KEY, TAMANHO * sizeof(Consulta), IPC_CREAT | 0600);
    printf("%d", id);

    exit_on_error(id, "Erro a criar shared memory");


}

