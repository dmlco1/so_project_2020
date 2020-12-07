#include "defines.h"

void main(){

	Mensagem m;
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

}
