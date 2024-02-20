#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "kbhit.h"
#include <stdlib.h>

int AX, BX, CX, DX, PC = 0;
char IR[100] = "VACIO";
char LineaLeida[100] = "VACIO";

void Prompt(WINDOW *comandos, int linea, char *comando) {
    mvwprintw(comandos, linea + 1, 2, "%d>> %s", linea, comando);
    wrefresh(comandos);
}

void Mensajes(WINDOW *mensajes, char *mensaje) {
    mvwprintw(mensajes, 3, 2, "%s", mensaje);
    wrefresh(mensajes);
}

void Registros(WINDOW *registros, int AX, int BX, int CX, int DX, int PC, char *IR, char *LineaLeida) {
    mvwprintw(registros, 2, 10, "AX: %d", AX);
    mvwprintw(registros, 2, 25, "BX: %d", BX);
    mvwprintw(registros, 4, 10, "CX: %d", CX);
    mvwprintw(registros, 4, 25, "DX: %d", DX);
    mvwprintw(registros, 6, 10, "PC: %d", PC);
    mvwprintw(registros, 6, 25, "IR: %s", IR);
    mvwprintw(registros, 8, 10, "Linea leida: %s", LineaLeida);
    wrefresh(registros);
}

void Errores(WINDOW *mensajes, int codigoError, char *comando, int *j) {
    if (codigoError == 104) {
        Mensajes(mensajes, "Saliendo del programa...");
        usleep(2000000); // Espera antes de salir
        exit(0); // Salir del programa
    }

    if (codigoError == 101) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Error: No se ingresó un nombre de archivo, uso: cargar <nombre_archivo>");
        memset(comando, 0, sizeof(comando));
        *j = 0;
    }

    if (codigoError == 102) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Error: El archivo no existe");
        memset(comando, 0, sizeof(comando)); // Limpiar el comando 
        *j = 0; // Reiniciar el contador de caractere
    }

    if (codigoError == 103) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Archivo leído exitosamente");
        memset(comando, 0, sizeof(comando)); // Limpiar el comando 
        *j = 0; // Reiniciar el contador de caractere
    }

    if (codigoError == 106) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Comando no reconocido");
        memset(comando, 0, sizeof(comando)); // Limpiar el comando 

        *j = 0; // Reiniciar el contador de caracteres
    }
}

void EjecutarInstruccion(WINDOW *registros, char *linea) {
    char instruccion[100], registro[100];
    int valor;

    // Leer la instrucción y el registro de la línea
    sscanf(linea, "%s %s %d", instruccion, registro, &valor);

    // Ejecutar la instrucción según el comando
    if (strcmp(instruccion, "MOV") == 0) {
        if (strcmp(registro, "AX") == 0) AX = valor;
        else if (strcmp(registro, "BX") == 0) BX = valor;
        else if (strcmp(registro, "CX") == 0) CX = valor;
        else if (strcmp(registro, "DX") == 0) DX = valor;
    } else if (strcmp(instruccion, "ADD") == 0) {
        if (strcmp(registro, "AX") == 0) AX += valor;
        // Aqui tengo que poner el resto de los registros
    }
    // Aqui debo poner el resto de las instrucciones
}

int Cargar(WINDOW *registros, char *nombre_archivo) {
    if (nombre_archivo[0] == '\0') { // Esto significa que no se ingresó un nombre de archivo
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
        // Ejecutar la instrucción de la línea
        EjecutarInstruccion(registros, linea);
        strcpy(LineaLeida, linea);

        Registros(registros, AX, BX, CX, DX, PC, IR, LineaLeida);
        usleep(1000000);
    }
    fclose(archivo);
    return 103; // Fin de archivo
}

int Enter(WINDOW *mensajes, char *comando) {
    char cmd[100];
    int height, width;
    getmaxyx(stdscr, height, width);
    WINDOW *registros = newwin(height / 2.4, width, 2 * height / 3.5, 0);
    wbkgd(registros, COLOR_PAIR(3));
    box(registros, 0, 0);
    mvwprintw(registros, 0, 2, "REGISTROS");
    char param1[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    char param2[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    // -------------- SECCION DE COMANDOS ---------------

    // ---- EXIT ----
    sscanf(comando, "%s", cmd); // Leer el comando (la primera palabra del comando)
    if (strcmp(cmd, "salir") == 0) {
        return 104; // Esto sale del programa porque se regresa 0 a la función main y el ciclo se rompe
    }

    // ---- CARGAR ----
    sscanf(comando, "%s", cmd); // Leer el comando
    if (strcmp(cmd, "cargar") == 0) {
        sscanf(comando, "%*s %s", param1); // Leer el primer parámetro
        int resultado = Cargar(registros, param1);
        return resultado;
    }

    return 106; // Comando no reconocido
}


int LineaComandos(WINDOW *comandos, WINDOW *mensajes, char *comando, int *j, int *linea) {
    int caracter = 0;
    
    if (kbhit()) { // kbhit() devuelve 1 si se ha presionado una tecla y 0 si no
        caracter = getch(); // Leer la tecla presionada (caracter)
        if (caracter != ERR) { // Verificar si se presionó una tecla
            if (caracter == '\n') { // Verificar si la tecla fue Enter
                comando[*j] = '\0'; // Finalizar el comando
                int codigoError = Enter(mensajes, comando);
                *j = 0; // Reiniciar el contador de caracteres
                (*linea)++; // Incrementar el contador de líneas
                Errores(mensajes, codigoError, comando, j);
            } else { // Agregar el carácter al búfer
                comando[*j] = caracter; // Guardar el carácter en el comando
                comando[(*j)+1]='\0';
                (*j)++; // Incrementar el contador de caracteres
            }
        }
        //Prompt(comandos, *linea, comando);
    }
    
    return 0;
}

int main(void) {
    initscr();  // Inicializar la pantalla
    start_color();
    init_color(1, 0, 0, 0);
    init_color(2, 250, 0, 150);
    init_pair(1, COLOR_WHITE, 1);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, 2);
    attron(COLOR_PAIR(1));

    cbreak(); // Deshabilitar el buffer de entrada
    noecho(); // No mostrar los caracteres ingresados
    
    int height, width;
    getmaxyx(stdscr, height, width);

    // Crear 3 ventanas para las áreas
    WINDOW *comandos = newwin(height / 2.3 , width, 0, 0);
    wbkgd(comandos, COLOR_PAIR(1));
    box(comandos, 0, 0);
    mvwprintw(comandos, 0, 2, "COMANDOS");


    WINDOW *mensajes = newwin(height / 7, width, height / 2.3, 0);
    wbkgd(mensajes, COLOR_PAIR(2));
    box(mensajes, 0, 0);
    mvwprintw(mensajes, 0, 2, "MENSAJES");
    attroff(COLOR_PAIR(1));

    WINDOW *registros = newwin(height / 2.4, width, 2 * height / 3.5, 0);
    wbkgd(registros, COLOR_PAIR(3));
    box(registros, 0, 0);
    mvwprintw(registros, 0, 2, "REGISTROS");

    WINDOW *deco = newwin(height / 10.5, width, 3.4 * height / 3.5, 0);
    wbkgd(deco, COLOR_PAIR(1));
    mvwprintw(deco, 0, 20, "Windos I . Version 1.3 . 2024");

    // Actualizar la pantalla para mostrar los cambios
    refresh();
    wrefresh(comandos);
    wrefresh(mensajes);
    wrefresh(registros);
    wrefresh(deco);
    

    char comando[1000] = {0}; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    int j = 0; // Contador de caracteres
    int linea = 0; // Contador de líneas

    Registros(registros, AX, BX, CX, DX, PC, IR, LineaLeida);

    Prompt(comandos, linea, comando);

    while (1) {
        LineaComandos(comandos, mensajes, comando, &j, &linea);
        Prompt(comandos, linea, comando);
        usleep(100000);
        
    }

    endwin();
    return 0;
}
