#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "kbhit.h"

int Cargar(char *nombre_archivo){
    if (nombre_archivo[0] == '\0') { //Esto significa que no se ingresó un nombre de archivo
        printf("Comandos> Modo de uso: cargar nombre_archivo\n");
        return 1;
    }

    FILE *archivo;
    archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura
    if (archivo == NULL) { // Si el archivo no existe
        printf("Comandos>El archivo %s no existe\n", nombre_archivo);
        return 1;
    }

    char linea[100]; // Esta variable almacenará cada línea del archivo
    while (fgets(linea, 100, archivo) != NULL) { // Leer cada línea del archivo
        printf("Comandos>\n%s", linea);
    }

    fclose(archivo); // Cerrar el archivo
    return 1;
}

int Enter(char *comando) {
    char cmd[100]; 
    char param1[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    char param2[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)

    // -------------- SECCION DE COMANDOS ---------------


    // ---- EXIT ----
    if (strcmp(comando, "salir") == 0) {
        printf("Comandos> Saliendo\n");
        return 0; // Esto sale del programa porque se regresa 0 a la función main y el ciclo se rompe
    }


    // ---- CARGAR ----
    
    sscanf(comando, "%s %s", cmd, param1); // Leer el comando y el primer parámetro
    if (strcmp(cmd, "cargar") == 0) {
        return Cargar(param1);
    }


    // ---- OTROS ----

    else {
        printf("Comandos> Comando no reconocido\n");
    }

    // --------------------------------------------------

    return 1;
}

int main(void) {
    char comando[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    int j = 0; // Contador de caracteres
    printf("Comandos>\n"); //Prompt

    while (1) {
        if (kbhit()) { // kbhit() devuelve 1 si se ha presionado una tecla y 0 si no
            comando[j++] = getchar(); // Leer la tecla presionada (caracter)
            comando[j] = '\0'; // Agregar el caracter nulo al final del comando
            
            if (comando[j-1] == 10) { // 10 es el código ASCII de Enter
                comando[j-1] = '\0';
                int Ejecutar = Enter(comando); // Procesar el comando
                if (Ejecutar == 0) {
                    break; // Salir del ciclo
                }
                j = 0; // Reiniciar
                comando[0] = '\0'; // Reiniciar
            } 
            
            if (comando[j-1] == 127) { // 127 es el código ASCII de backspace
                comando[j-2] = '\0'; // Eliminar el último caracter
                j -= 2; // Retroceder dos posiciones
                if (j < 0) j = 0; // Evita que j sea negativo
            }

            printf("Comandos> %s\n",  comando); //Prompt
            usleep (100000); // Delay de 100 ms
        }
    }

    return 0;
}
