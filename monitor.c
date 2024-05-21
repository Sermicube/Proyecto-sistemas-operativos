/**************************
Autores: Sergio Cuellar y Renato Negrete
Fecha: 26/04/2024
Univeridad: Javeriana
Materia: Sistemas Operativos
Tema: Proyecto
Codigo del monitor
***************************/

#include "monitor.h"
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

// Se define un tamaño maximo para el buffer
#define MAX_TAM 100000 

// Se declaran los buffers junto con un contador de cada uno
double tempBuffer[MAX_TAM];
int countTemp = 0;
double phBuffer[MAX_TAM];
int countPh = 0;
int TAM_BUFFER;

int acaboTemp = 0;
int acaboPh = 0;

// Se declaran los semaforos
sem_t semTempLleno, semTempVacio, semPhLleno, semPhVacio;
pthread_mutex_t mutexTemp, mutexPh;

void *funcionRecolector(void *arg) {
  char *pipe = (char *)arg; // Se obtiene el nombre del pipe

  int fd = open(pipe, O_RDONLY); // Se abre el pipe en modo solo lectura

  if (fd == -1) {
    printf("Error al abrir el pipe nominal\n"); // Se verifica que el pipe se abrio correctamente
    return NULL;
  }

  size_t bytes_leidos;
  while(1) {
    char buffer[100]; // Se define un buffer donde se guardan los datos leidos del pipe
    bytes_leidos = read(fd, buffer, 100); // Se lee del pipe
    if(bytes_leidos <= 0){
      if (errno == EPIPE) { 
        printf("El sensor termino.\n");
        break;
      } else {
        perror("read");
        exit(EXIT_FAILURE);
      }
    }
    char *token1 = strtok(buffer, " "); // Se hace la tokenizacion del buffer
    char *token2 = strtok(NULL, " "); // Obtenemos el valor numérico como cadena
    float dato = atof(token2); // Se transforma la cadena donde esta el valor numerico a un flotante
    if (dato > 0) { // Se verifica que el dato sea mayor a cero
      if (strcmp(token1, "TEMP") == 0) { // Se revisa si el dato es de temperatura
        sem_wait(&semTempVacio); // Se hace un sem_wait 
        pthread_mutex_lock(&mutexTemp);
        tempBuffer[countTemp] = dato; // Se coloca el dato en el buffer
        countTemp++; // Se aumenta al contador
        printf("Dato de temperatura guardado en el buffer: %f\n", tempBuffer[countTemp-1]);
        pthread_mutex_unlock(&mutexTemp);
        sem_post(&semTempLleno); // Se hace un sem_post
      } else if (strcmp(token1, "PH") == 0) { // Se revisa si el dato es de ph
        sem_wait(&semPhVacio); // Se hace un sem_wait
        pthread_mutex_lock(&mutexPh);
        phBuffer[countPh] = dato; // Se guarda el dato en el buffer
        countPh++; // Se aumenta el contador
        printf("Dato de pH guardado en el buffer: %f\n", phBuffer[countPh-1]);
        pthread_mutex_unlock(&mutexPh);
        sem_post(&semPhLleno); // Se hace un sem_post
      }
    }
  }
  sleep(10);
  acaboTemp = 1;
  acaboPh = 1;
  close(fd); // Se cierra el pipe
  return NULL;
}

void *funcionTemperatura(void *arg) {
  char *file = (char *)arg; // Se obtiene el nombre del archivo
  FILE *archivo = fopen(file, "w"); // Se abre el archivo en modo escritura

  if (!archivo) {
    printf("No se pudo abrir el archivo\n"); // Se verifica que el archivo se pudo abrir
    return NULL;
  }
  while(1){
    sem_wait(&semTempLleno); // Se hace un sem_wait
    pthread_mutex_lock(&mutexTemp);
    if(acaboTemp == 1){
      pthread_mutex_unlock(&mutexTemp);
      sem_post(&semTempLleno);
      break;
    }
    
    float y = tempBuffer[countTemp - 1]; // Se obtiene el ultimo dato del buffer
    countTemp--; // Se resta al contador
    pthread_mutex_unlock(&mutexTemp);
    sem_post(&semTempVacio); // Se hace un sem_post
    if (y < 20 || y > 31.6) { // Se verifica que la temperatura este dentro del rango
      printf("Temperatura fuera de rango\n"); // Si la temperatura esta fuera del rango se imprime el mensaje
    }
    time_t now;
    time(&now); // Se obtiene el tiempo actual
    char *time_str = ctime(&now); // Transforma el tiempo a una cadena de caracteres
    fprintf(archivo, "%f\t%s", y, time_str); // Se escriben los datos en el archivo
  }

  fclose(archivo); // Se cierra el archivo

  return NULL;
}

void *funcionPh(void *arg) {
  char *file = (char *)arg; // Se obtiene el nombre del archivo
  FILE *archivo = fopen(file, "w"); // Se abre el archivo en modo escritura

  if (!archivo) {
    printf("No se pudo abrir el archivo\n"); // Se verifica que el archivo se pudo abrir
    return NULL;
  }
  while(1){
    sem_wait(&semPhLleno); // Se hace un sem_wait
    pthread_mutex_lock(&mutexPh);
    if(acaboPh == 1){
      pthread_mutex_unlock(&mutexPh);
      sem_post(&semPhLleno);
      break;
    }
    float y = phBuffer[countPh - 1]; // Se obtiene el ultimo dato del buffer
    countPh--; // Se resta al contador
    pthread_mutex_unlock(&mutexPh);
    sem_post(&semPhVacio); // Se hace un sem_post
    if (y < 6.0 || y > 8.0) { // Se verifica que el ph este dentro del rango
      printf("PH fuera de rango\n"); // Si el ph esta fuera del rango se imprime el mensaje
    }
    time_t now;
    time(&now); // Se obtiene el tiempo actual
    char *time_str = ctime(&now); // Transforma el tiempo a una cadena de caracteres
    fprintf(archivo, "%f\t%s", y, time_str); // Se escriben los datos en el archivo
  }

  fclose(archivo); // Se cierra el archivo

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 9) {
    printf("Uso: %s -b tam_buffer -t file_temp -h file_ph -p pipe_nominal\n", argv[0]); // Se verifica que haya la cantidad correcta de argumentos
    return EXIT_FAILURE;
  }

  
  TAM_BUFFER = atoi(argv[2]); // Se obtiene el tamaño del buffer
  char *file_ph = argv[6]; // Se obtiene el nombre del archivo de ph
  char *pipe_nominal = argv[8]; // Se obtiene el nombre del pipe nominal
  char *file_temp = argv[4]; // Se obtiene el nombre del archivo de temperatura

  sem_init(&semTempLleno, 0, 0);
  sem_init(&semTempVacio, 0, TAM_BUFFER); // Se inicializan todos los semaforos
  sem_init(&semPhLleno, 0, 0);
  sem_init(&semPhVacio, 0, TAM_BUFFER);
  pthread_mutex_init(&mutexPh, NULL);
  pthread_mutex_init(&mutexTemp, NULL);

  pthread_t h_recolector, h_temperatura, h_ph; // Se declaran los hilos

  if (pthread_create(&h_recolector, NULL, &funcionRecolector, (void *)pipe_nominal) != 0) { // Se crea el hilo de recolector con su respectiva funcion
    return -1;
  }
  if (pthread_create(&h_temperatura, NULL, &funcionTemperatura, (void *)file_temp) != 0) { // Se crea el hilo de temperatura con su respectiva funcion
    return -1;
  }
  if (pthread_create(&h_ph, NULL, &funcionPh, (void *)file_ph) != 0) { // Se crea el hilo de ph con su respectiva funcion
    return -1;
  }

  if (pthread_join(h_recolector, NULL) != 0) { // Se hace pthread_join de hilo recolector
    return -1;
  }
  if (pthread_join(h_temperatura, NULL) != 0) { // Se hace pthread_join de hilo de temperatura
    return -1;
  }
  if (pthread_join(h_ph, NULL) != 0) { // Se hace pthread_join de hilo de ph
    return -1;
  }

  sem_destroy(&semTempLleno);
  sem_destroy(&semTempVacio); // Se destruyen todos los semaforos
  sem_destroy(&semPhLleno);
  sem_destroy(&semPhVacio);
  pthread_mutex_destroy(&mutexPh);
  pthread_mutex_destroy(&mutexTemp);

  return 0;
}