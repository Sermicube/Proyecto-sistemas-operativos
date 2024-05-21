# Sensor-Monitor
Este es un ejercicio donde se muestra el funcionamiento de un sensor de agua y temperatura y un monitor el cual ve los datos obtenidos y los evalua.

El sensor lee datos de un archivo de texto y manda estos datos a traves de un pipe al monitor para hacer la respectiva evaluación de los datos.

El monitor recibe los datos que manda el sensor y los evalua dependiendo del tipo de dato que es, después de evaluarlo se escribe en un archivo de texto el dato junto con la fecha y la hora actual.

El sensor usa un archivo fifo, que funciona como un pipe, para la comunicación entre el proceso de sensor y el proceso de monitor. El monitor hace uso de tres hilos, uno para leer los datos enviados por el monitor, otro para evaluar los datos de tipo temperatura y escribir los datos en el archivo de salida de temperatura y el ultimo para evaluar los datos de tipo ph y escribir los datos en el archivo de salida de ph.

Para ejecutar el codigo y ver el funcionamiento del mismo existe un archivo Makefile haciendo uso del comando "make run" el la terminal se ejecutan al mismo tiempo dos sensores de ph, dos sensores de temperatura y el monitor.
