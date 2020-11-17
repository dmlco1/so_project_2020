#ifndef __CONSULTA_H__
#define __CONSULTA_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{

    int tipo; // tipo de consulta: 1-Normal, 2-COVID-19, 3-Urgente
    char descricao[100]; //descricao da consulta
    int pid_consulta; // PID do processo que quer fazer a consulta

}Consulta;

#endif
