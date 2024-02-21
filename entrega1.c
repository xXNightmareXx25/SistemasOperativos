#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "kbhit.h"
#include <ctype.h>

// Estructura para el CPU
typedef struct {
    int AX;
    int BX;
    int CX;
    int DX;
    int PC;
    char IR[100];
    char LineaLeida[100];
} PCB;

void Prompt(WINDOW *comandos, int linea, char *comando) {
    mvwprintw(comandos, linea + 1, 2, "%d>> %s", linea, comando);
    wrefresh(comandos);
}

void Mensajes(WINDOW *mensajes, char *mensaje) {
    mvwprintw(mensajes, 3, 2, "%s", mensaje);
    wrefresh(mensajes);
}

void Registros(WINDOW *registros, PCB *pcb) {
    mvwprintw(registros, 2, 10, "AX: %d        ", pcb->AX);
    mvwprintw(registros, 2, 25, "BX: %d        ", pcb->BX);
    mvwprintw(registros, 4, 10, "CX: %d        ", pcb->CX);
    mvwprintw(registros, 4, 25, "DX: %d        ", pcb->DX);
    mvwprintw(registros, 6, 10, "PC: %d        ", pcb->PC);
    mvwprintw(registros, 6, 25, "IR: %s        ", pcb->IR);
    mvwprintw(registros, 8, 10, "Linea leida: %s        ", pcb->LineaLeida);
    wrefresh(registros);
}

void Errores(WINDOW *mensajes, int codigoError, char *comando, int *j) {
    if (codigoError == 104) {
        Mensajes(mensajes, "Saliendo del programa...");
        usleep(2000000); // Espera antes de salir
        exit(0); // Salir del programa
    }

    // ----- ERRORES PARA CARGAR -----

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
    //--------------------------------------------------------------------------------



    // ----- ERRORES PARA COMANDOS -----

    if (codigoError == 106) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Comando no reconocido");
        memset(comando, 0, sizeof(comando)); // Limpiar el comando 

        *j = 0; // Reiniciar el contador de caracteres
    }

    //--------------------------------------------------------------------------------

}


int ErroresInstrucciones(WINDOW *mensajes, int codigoError) {
    if (codigoError == 107) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Error: Instrucción no reconocida");
    }

    if (codigoError == 108) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Error: División por cero o registro no válido");
    }

    if (codigoError == 109) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Terminando la ejecución del programa...");        
    }
}

int EjecutarInstruccion(WINDOW *registros, WINDOW *mensajes, PCB *pcb, char *linea) {
    char instruccion[100], registro[100];
    int valor;
    int codigoError = 0;

    // Leer la instrucción y el registro de la línea
    sscanf(linea, "%s %s %d", instruccion, registro, &valor);

    // Convertir la línea a mayúsculas
    for(int i = 0; linea[i]; i++){
        linea[i] = toupper(linea[i]);
    }
    
    strcpy(pcb->IR, linea); // Almacenar la línea en el registro IR
    // Incrementar el PC
    pcb->PC++;

    // ----- INSTRUCCIONES PARA END -----
    if (strcmp(instruccion, "END") == 0) {
        codigoError = 109;
        ErroresInstrucciones(mensajes, codigoError);
        return 109; // Fin de archivo
    } 

    // ----- INSTRUCCIONES PARA MOV -----
    else if (strcmp(instruccion, "MOV") == 0) {
        if (strcmp(registro, "AX") == 0) pcb->AX = valor;
        else if (strcmp(registro, "BX") == 0) pcb->BX = valor;
        else if (strcmp(registro, "CX") == 0) pcb->CX = valor;
        else if (strcmp(registro, "DX") == 0) pcb->DX = valor;
    } 
    
    // ----- INSTRUCCIONES PARA ADD -----
    else if (strcmp(instruccion, "ADD") == 0) {
        if (strcmp(registro, "AX") == 0) pcb->AX += valor;
        else if (strcmp(registro, "BX") == 0) pcb->BX += valor;
        else if (strcmp(registro, "CX") == 0) pcb->CX += valor;
        else if (strcmp(registro, "DX") == 0) pcb->DX += valor;
    } 
    
    // ----- INSTRUCCIONES PARA SUB -----
    else if (strcmp(instruccion, "SUB") == 0) {
        if (strcmp(registro, "AX") == 0) pcb->AX -= valor;
        else if (strcmp(registro, "BX") == 0) pcb->BX -= valor;
        else if (strcmp(registro, "CX") == 0) pcb->CX -= valor;
        else if (strcmp(registro, "DX") == 0) pcb->DX -= valor;
    } 
    
    // ----- INSTRUCCIONES PARA MUL -----
    else if (strcmp(instruccion, "MUL") == 0) {
        if (strcmp(registro, "AX") == 0) pcb->AX *= valor;
        else if (strcmp(registro, "BX") == 0) pcb->BX *= valor;
        else if (strcmp(registro, "CX") == 0) pcb->CX *= valor;
        else if (strcmp(registro, "DX") == 0) pcb->DX *= valor;
    } 

    // ----- INSTRUCCIONES PARA DIV -----
    else if (strcmp(instruccion, "DIV") == 0) {
        if (valor == 0) {
            codigoError = 108;
            ErroresInstrucciones(mensajes, codigoError);
            return 1; // Error de división por cero
        } else {
            if (strcmp(registro, "AX") == 0) pcb->AX /= valor;
            else if (strcmp(registro, "BX") == 0) pcb->BX /= valor;
            else if (strcmp(registro, "CX") == 0) pcb->CX /= valor;
            else if (strcmp(registro, "DX") == 0) pcb->DX /= valor;
        }
    } 
    
    // ----- INSTRUCCIONES PARA INC -----
    else if (strcmp(instruccion, "INC") == 0) {
        if (strcmp(registro, "AX") == 0) pcb->AX++;
        else if (strcmp(registro, "BX") == 0) pcb->BX++;
        else if (strcmp(registro, "CX") == 0) pcb->CX++;
        else if (strcmp(registro, "DX") == 0) pcb->DX++;
    } 
    
    // ----- INSTRUCCIONES PARA DEC -----
    else if (strcmp(instruccion, "DEC") == 0) {
        if (strcmp(registro, "AX") == 0) pcb->AX--;
        else if (strcmp(registro, "BX") == 0) pcb->BX--;
        else if (strcmp(registro, "CX") == 0) pcb->CX--;
        else if (strcmp(registro, "DX") == 0) pcb->DX--;
    }



    // ERROR
    else {
        codigoError = 107;
        ErroresInstrucciones(mensajes, codigoError);
        return codigoError;
    }

    codigoError = 0;
    return 0; // No hay error
}

int Cargar(WINDOW *registros,WINDOW *mensajes, PCB *pcb, char *nombre_archivo) {
    if (nombre_archivo[0] == '\0') { // Esto significa que no se ingresó un nombre de archivo
        return 101;
    }

    FILE *archivo;
    archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura
    if (archivo == NULL) { // Si el archivo no existe
        return 102;
    }

    char linea[100]; // Esta variable almacenará cada línea del archivo
    pcb->PC = 0; // Inicializar PC en 0
    while (fgets(linea, 100, archivo) != NULL) { // Leer cada línea del archivo
        linea[strcspn(linea, "\n")] = '\0'; // Eliminar el salto de línea (Para que se vea bonito en el prompt)
        // Ejecutar la instrucción de la línea
        //convierte la linea a mayusculas
        for(int i = 0; linea[i]; i++){
            linea[i] = toupper(linea[i]);
        }
        int codigoError = EjecutarInstruccion(registros,mensajes, pcb, linea);

        if (codigoError == 109) {
            fclose(archivo);
            return 109; // Fin de archivo
        }

        if (codigoError == 103) {
            fclose(archivo);
            return 103; // Fin de archivo
        }
        
        if (codigoError != 0) {
            fclose(archivo);
            return codigoError;
        }

        strcpy(pcb->LineaLeida, linea);

        Registros(registros, pcb);
        usleep(1000000);

        
    }
    fclose(archivo);
    return 103; // Fin de archivo
}


int Enter(WINDOW *mensajes,WINDOW *registros, char *comando, PCB *pcb) {
    char cmd[100];
    int height, width;
    getmaxyx(stdscr, height, width);
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
        int resultado = Cargar(registros,mensajes, pcb, param1);
        memset(comando, 0, sizeof(comando)); // Limpiar el comando
        return resultado;
    }

    return 106; // Comando no reconocido
}

int LineaComandos(WINDOW *comandos, WINDOW *mensajes,WINDOW *registros, char *comando, int *j, int *linea, PCB *pcb) {
    int caracter = 0;
    
    if (kbhit()) { // kbhit() devuelve 1 si se ha presionado una tecla y 0 si no
        caracter = getch(); // Leer la tecla presionada (caracter)
        if (caracter != ERR) { // Verificar si se presionó una tecla
            if (caracter == '\n') { // Verificar si la tecla fue Enter
                comando[*j] = '\0'; // Finalizar el comando
                int codigoError = Enter(mensajes, registros, comando, pcb);
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

    PCB pcb; // Crear una instancia de la estructura PCB
    memset(&pcb, 0, sizeof(PCB)); // Inicializar la estructura a 0

    Prompt(comandos, linea, comando);

    Registros(registros, &pcb);

    while (1) {
        LineaComandos(comandos, mensajes, registros, comando, &j, &linea, &pcb);
        Prompt(comandos, linea, comando);

    }

    endwin();
    return 0;
}
