/**************************
Autores: Sergio Cuellar y Renato Negrete
Fecha: 26/04/2024
Univeridad: Javeriana
Materia: Sistemas Operativos
Tema: Proyecto
Codigo de encabezado monitor
***************************/


#ifndef __MONITOR_H__
#define __MONITOR_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#define MAX_TAM 100000

extern double tempBuffer[];
extern int countTemp;
extern double phBuffer[];
extern int countPh;
extern int TAM_BUFFER;

extern sem_t semTemp, semPh;

void *funcionRecolector(void *arg);
void *funcionTemperatura(void *arg);
void *funcionPh(void *arg);
int llamarMonitor(char *argv[]);

#endif //__MONITOR_H__

