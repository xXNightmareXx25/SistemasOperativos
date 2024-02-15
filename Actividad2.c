#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "kbhit.h"
#include <stdlib.h>



int Cargar(char *nombre_archivo){
    if (nombre_archivo[0] == '\0') { //Esto significa que no se ingresó un nombre de archivo
        return 101;
    }

    FILE *archivo;
    archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura
    if (archivo == NULL) { // Si el archivo no existe
        return 102;
    }

    char linea[100]; // Esta variable almacenará cada línea del archivo
    while (fgets(linea, 100, archivo) != NULL) { // Leer cada línea del archivo
        linea[strcspn(linea, "\n")] = '\0'; // Eliminar el salto de línea (Para que se vea bonito en el prompt)
        Prompt(linea); // Mandar la línea al prompt
    }
    fclose(archivo);
    return 103; // Fin de archivo

}

int Enter(char *comando) {
    char cmd[100]; 
    char param1[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    char param2[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)

    // -------------- SECCION DE COMANDOS ---------------


    // ---- EXIT ----
    sscanf(comando, "%s", cmd); // Leer el comando (la primera palabra del comando)
    if (strcmp(cmd, "salir") == 0) {
        return 104; // Esto sale del programa porque se regresa 0 a la función main y el ciclo se rompe
    }


    // ---- CARGAR ----
    
    sscanf(comando, "%s %s", cmd, param1); // Leer el comando y el primer parámetro
    if (strcmp(cmd, "cargar") == 0) {
    int resultado = Cargar(param1);
    return resultado;
  }

    return 106; // Comando no reconocido
}

int Prompt(char *comando) {
    printf("Comandos> %s\n",  comando); //Prompt
    return 0;
}

int LineaComandos(char *comando, int *j) {
    
    if (kbhit()) { // kbhit() devuelve 1 si se ha presionado una tecla y 0 si no
            comando[(*j)++] = getchar(); // Leer la tecla presionada (caracter)
            comando[(*j)] = '\0'; // Agregar el caracter nulo al final del comando
            
            if (comando[*j-1] == 10) { // 10 es el código ASCII de Enter
                int codigoError = Enter(comando); // Ejecutar el comando
                comando[*j-1] = '\0';

                
                if (codigoError == 104) {
                    exit(0); // Salir del programa
                }

                if (codigoError == 101) {
                    strcpy(comando, "Error: No se ingresó un nombre de archivo, uso: cargar <nombre_archivo>");
                    Prompt(comando);
                }

                if (codigoError == 102) {
                    strcpy(comando, "Error: El archivo no existe");
                    Prompt(comando);
                }

                if (codigoError == 103) {
                    strcpy(comando, "Archivo leido exitosamente");
                    Prompt(comando);
                }

                if (codigoError == 106) {
                    strcpy(comando, "Comando no reconocido");
                    Prompt(comando);
                }

                *j = 0; // Reiniciar el contador de caracteres
                comando[0] = '\0'; // Reiniciar

            } 
            
            if (comando[*j-1] == 127) { // 127 es el código ASCII de backspace
                comando[*j-2] = '\0'; // Eliminar el último caracter
                *j -= 2; // Retroceder dos posiciones
                if (*j < 0) *j = 0; // Evita que j sea negativo
            }

            Prompt(comando); // Imprimir el prompt (comando)
            
            usleep (100000); // Delay de 100 ms
    }

    return 0;
}

int main(void) {
    char comando[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    int j = 0; // Contador de caracteres
    printf("Comandos>\n"); //Prompt

    while (1) {
        LineaComandos(comando, &j);
    }

    return 0;
}
