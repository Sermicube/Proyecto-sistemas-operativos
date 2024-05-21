/**************************
Autores: Sergio Cuellar y Renato Negrete
Fecha: 26/04/2024
Univeridad: Javeriana
Materia: Sistemas Operativos
Tema: Proyecto
Codigo del sensor
***************************/

#include "sensor.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>



void leerArchivo(char *archivo, int tipo, char *pipe, int tiempo){
  size_t bytes_escritos;
  
  FILE* file = fopen(archivo, "r");
  if (!archivo) {
    printf("No se pudo abrir el archivo\n"); // Se verifica que el archivo se pudo abrir
    return;
  }

  int pipe_fd = open(pipe, O_WRONLY);
  if (pipe_fd == -1) {
    perror("Error al abrir el pipe para escritura"); // Se verifica que el pipe se pudo abrir
    exit(EXIT_FAILURE);
  }

  char tipo_str[5];
  if(tipo == 1){
    strcpy(tipo_str, "TEMP"); // Se escribe TEMP si el tipo es 1
  } else if(tipo == 2) {
    strcpy(tipo_str, "PH"); // Se escribe PH si el tipo es 2
  } else {
    printf("El tipo de sensor ingresado no es valido\n");
    return;
  }

  char enviar[100];
  float medida;
  char medida_str[6];
  while(fscanf(file, "%f", &medida) != EOF){ // Se lee cada medida del archivo
    snprintf(enviar, 100, "%s %f", tipo_str, medida); // Se forma el mensaje que se va a enviar en el pipe
    bytes_escritos = write(pipe_fd, enviar, strlen(enviar+1)); // Se escribe el mensaje en el pipe
    if (bytes_escritos == -1) {
      perror("Error al escribir en el pipe");
      exit(EXIT_FAILURE);
    }
    usleep(tiempo * 1000000);
    //printf("Datos escritos en el pipe: %s\n", enviar);
  }

  close(pipe_fd);
  fclose(file);
}

//$ ./sensor –s tipo-sensor –t tiempo –f archivo –p pipe nominal

int main(int argc, char *argv[]) {

  struct datosEntrada *datos=(struct datosEntrada *)malloc(sizeof(struct datosEntrada));

  if(argc!=9){
    printf("Argumentos no válidos\n $ ./sensor –s tipo-sensor –t tiempo –f archivo –p pipe nominal\n"); // Se verifica la cantidad de argumentos correcta
    return -1;
  }
  for(int i=i;i<argc;i++){
    if(i+1!=argc){
      if(strcmp(argv[i],"-s")==0){ // Se verifica si la flag es -s
        char *k=argv[i+1];
        datos->tipo=atoi(k);
        if(datos->tipo!=1&&datos->tipo!=2){
          printf("\n\n\tEste tipo de sensor no existe!!!\n\n");
          return 0;
        }
        i++;
      }
      if(strcmp(argv[i],"-t")==0){ // Se verifica si la flag es -t
        char *z=argv[i+1];
        datos->tiempo=atoi(z);
        i++;
      }
      if(strcmp(argv[i],"-f")==0){ // Se verifica si la flag es -f
        char *d=argv[i+1];
        datos->archivo=d;
        i++;
      }
      if(strcmp(argv[i],"-p")==0){ // Se verifica si la flag es -p
        char *f=argv[i+1];
        datos->pipe=f;
        i++;
      }
    }
  }

  /*if (mkfifo(datos->pipe, 0666) == -1) {
    perror("Error al crear el pipe");
    exit(EXIT_FAILURE);
  }*/
  leerArchivo(datos->archivo, datos->tipo, datos->pipe, datos->tiempo);

  return 0;
}
