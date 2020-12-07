#include "defines.h"

void main(){

	Consulta c;
	char tipo[10];

	// c1) c2) Introduzir os dados de consulta
	printf("Tipo de Consulta: ");
	fgets(tipo, 10, stdin);
	c.tipo = atoi(tipo);

	printf("Descricao: ");
    fgets(c.descricao, 100, stdin);
    c.descricao[strlen(c.descricao)-1] = 0;

	c.pid_consulta = getpid();
	c.status = 1;

	

}
