/**************************
Autores: Sergio Cuellar y Renato Negrete
Fecha: 26/04/2024
Univeridad: Javeriana
Materia: Sistemas Operativos
Tema: Proyecto
codigo de encabezado para sensor
***************************/

#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


struct datosEntrada{
  int tipo;
  int tiempo;
  char *archivo;
  char *pipe;
};

void leerArchivo(char *archivo, int tipo, char *pipe, int tiempo);

#endif //__SENSOR_H__