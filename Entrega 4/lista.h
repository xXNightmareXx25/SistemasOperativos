#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int Pmin=0;

//-----------------------------------------------------------
//INCLUYE DOCUMENTACION PARA NUESTRAS LISTAS, PARA NO HACERNOS BOLAS 
//-----------------------------------------------------------

/*
    * listaExtraeInicio.
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista
    * 
    * Valor de retorno:
    * - Puntero al primer nodo extraído
    * 
    * Objetivo:
    * - Extraer el primer nodo de la lista
    * 
    * Descripción:
    * - Verifica si la lista está vacía.
    * - Si la lista está vacía, retorna NULL.
    * - Si la lista no está vacía, extrae el primer nodo de la lista y actualiza el puntero al primer nodo.
    * - Retorna el puntero al primer nodo extraído.
    */
struct PCB* listaExtraeInicio(struct PCB **PrimerNodo) {
    // Verifica si la lista está vacía
    if (*PrimerNodo == NULL) {
        return NULL;
    }

    // Extrae el primer nodo de la lista
    struct PCB* NodoMinimo = *PrimerNodo;
    // Actualiza el puntero al primer nodo
    struct PCB* Anterior = NULL;
    // Actualiza el puntero al primer nodo
    struct PCB* Actual = *PrimerNodo;

    // Recorre la lista para encontrar el nodo con la prioridad más baja
    while (Actual->sig != NULL) {
        if (Actual->sig->P < NodoMinimo->P) {
            NodoMinimo = Actual->sig;
            Anterior = Actual;
        }
        Actual = Actual->sig;
    }

    // Extrae el nodo con la prioridad más baja
    if(Anterior == NULL){
        *PrimerNodo = NodoMinimo->sig;
    }else{
        Anterior->sig = NodoMinimo->sig;
    }

    Pmin = NodoMinimo->P;

    return NodoMinimo;
}

/*
    * listaExtraeInicio.
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista
    * 
    * Valor de retorno:
    * - Puntero al primer nodo extraído
    * 
    * Objetivo:
    * - Extraer el primer nodo de la lista
    * 
    * Descripción:
    * - Verifica si la lista está vacía.
    * - Si la lista está vacía, retorna NULL.
    * - Si la lista no está vacía, extrae el primer nodo de la lista y actualiza el puntero al primer nodo.
    * - Retorna el puntero al primer nodo extraído.
    */

int MinimaPrioridad(){
    return Pmin;
}
/*
    * LiberaListos
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista
    * 
    * Objetivo:
    * - Liberar la memoria de la lista de procesos listos
    * 
    * Descripción:
    * - Recorre la lista de procesos listos y libera la memoria de cada nodo.
    * - Cierra el archivo asociado a cada nodo.
    * - Actualiza el puntero al primer nodo a NULL.
    */
void LiberaListos(struct PCB **PrimerNodo) {
    // Recorre la lista de procesos listos
    struct PCB *Actual = *PrimerNodo;
    struct PCB *Aux;

    // Libera la memoria de cada nodo
    while (Actual != NULL) {
        Aux = Actual;
        Actual = Actual->sig;
        fclose(Aux->programa);
        free(Aux);
    }

    *PrimerNodo = NULL;
}

/*
    * LiberaTerminados
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista
    * 
    * Objetivo:
    * - Liberar la memoria de la lista de procesos terminados
    * 
    * Descripción:
    * - Recorre la lista de procesos terminados y libera la memoria de cada nodo.
    * - Actualiza el puntero al primer nodo a NULL.
    */
void LiberaTerminados(struct PCB **PrimerNodo) {
    // Recorre la lista de procesos terminados
    struct PCB *Actual = *PrimerNodo;
    struct PCB *Aux;

    // Libera la memoria de cada nodo
    while (Actual != NULL) {
        Aux = Actual;
        Actual = Actual->sig;
        free(Aux);
    }

    // Actualiza el puntero al primer nodo
    *PrimerNodo = NULL;
}

/*
    * InsertaNuevo
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista
    * - ejecucion: Puntero al nodo que se va a insertar
    * 
    * Objetivo:
    * - Insertar un nuevo nodo al final de la lista
    * 
    * Descripción:
    * - Crea un nuevo nodo y copia los datos del nodo "ejecucion" en él.
    * - Inserta el nuevo nodo al final de la lista.
    * - Si la lista está vacía, el nuevo nodo se convierte en el primer nodo.
    */
void InsertarNuevo(struct PCB **PrimerNodo, struct PCB *ejecucion) {
    // Crea un nuevo nodo
    struct PCB *nuevoNodo = (struct PCB*)malloc(sizeof(struct PCB));
    // Verifica si se pudo reservar memoria
    if(nuevoNodo == NULL){
        return;
    }
    
    // Copia los datos del nodo "ejecucion" en el nuevo nodo

    *nuevoNodo = *ejecucion;
    nuevoNodo->sig = NULL;

    // Inserta el nuevo nodo al final de la lista
    if (*PrimerNodo == NULL) {
        *PrimerNodo = nuevoNodo;
        return;
    }

    // Recorre la lista hasta llegar al último nodo
    struct PCB *Ultimo = *PrimerNodo;
    while (Ultimo->sig != NULL) {
        Ultimo = Ultimo->sig;
    }

    // Inserta el nuevo nodo al final de la lista
    Ultimo->sig = nuevoNodo;
}


void AsignarMarcos(struct PCB *nuevoNodo, FILE *SWAP, char *nombrePrograma, int Repetido) {
    if (Repetido == -1) {
        int marcosAsignados = 0;
        for (int i = 0; i < TMP_SIZE_MAX; i++) {
            if (TablaTMS[i].PID == 0) {
                TablaTMS[i].marco = i;
                TablaTMS[i].PID = nuevoNodo->PID;
                nuevoNodo->TMP[marcosAsignados] = i;
                marcosAsignados++;
                if (marcosAsignados == nuevoNodo->TmpSize) {
                    break;
                }
            }
        }

        if (marcosAsignados < nuevoNodo->TmpSize) {
            // Manejar el caso donde no hay suficientes marcos libres
            for(int i = 0; i < TMP_SIZE_MAX; i++){
                if(TablaTMS[i].PID == nuevoNodo->PID){
                    TablaTMS[i].PID = 0;
                }
            }
            return;
        }

        char linea[32];
        FILE *ArchivoCargado = fopen(nombrePrograma, "r");
        if (ArchivoCargado != NULL) {
            int PCarchivo = 0;
            while (fgets(linea, sizeof(linea), ArchivoCargado) != NULL) {
                if(strcmp(linea, "\n") == 0){
                    continue;
                }
                int Marco = PCarchivo / 16;
                int Offset = PCarchivo % 16;
                int MarcoSWAP = nuevoNodo->TMP[Marco];
                int DireccionReal = (MarcoSWAP << 4) | Offset;
                int DezplazamientoSWAP = ((MarcoSWAP * 16 + Offset) * 32);

                char instruccion[32] = {0};
                strncpy(instruccion, linea, 32);

                fseek(SWAP, DezplazamientoSWAP, SEEK_SET);
                fwrite(instruccion, sizeof(char), 32, SWAP);

                PCarchivo++;
            }
            fclose(ArchivoCargado);
        }
    } else {
        for (int i = 0; i < TMP_SIZE_MAX; i++) {
            if (TablaTMS[i].PID == Repetido) {
                TablaTMS[i].marco = i;
                TablaTMS[i].PID = nuevoNodo->PID;
                nuevoNodo->TMP[i] = i;
            }
        }
    }
}

void InsertarTemporal(struct PCB **PrimerNodo, char *nombrePrograma, int PBase, int IDUs) {
    struct PCB *nuevoNodo = (struct PCB*)malloc(sizeof(struct PCB));
    // Verifica si se pudo reservar memoria
    if(nuevoNodo == NULL){
        return;
    }
    // Copia los datos del nodo "ejecucion" en el nuevo nodo
    nuevoNodo->programa = fopen(nombrePrograma, "r");
    // Verifica si se pudo abrir el archivo
    if (nuevoNodo->programa == NULL) {
        free(nuevoNodo);
        return;
    }
    // Copia el nombre del archivo
    strcpy(nuevoNodo->fileName, nombrePrograma);
    // Inicializa los registros del CPU

    nuevoNodo->AX = 0;
    nuevoNodo->BX = 0;
    nuevoNodo->CX = 0;
    nuevoNodo->DX = 0;
    nuevoNodo->PC = 0;
    nuevoNodo->KCPU = 0;
    nuevoNodo->UID = IDUs;
    nuevoNodo->P= PBase;
    nuevoNodo->KCPUxU=0;
    strcpy(nuevoNodo->IR, "");
    nuevoNodo->TmpSize = 0;
    // Inserta el nuevo nodo al final de la lista
    
    nuevoNodo->sig = NULL;
    

    if (*PrimerNodo == NULL) {
        *PrimerNodo = nuevoNodo;
        return;
    }
    // Recorre la lista hasta llegar al último nodo

    struct PCB *Ultimo = *PrimerNodo;
    // Inserta el nuevo nodo al final de la lista
    while (Ultimo->sig != NULL) {
        Ultimo = Ultimo->sig;
    }

    Ultimo->sig = nuevoNodo;
}

/*
    * Insertar.
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista
    * - nombrePrograma: Nombre del archivo del programa
    * - PBase: Prioridad base del proceso
    * - IDUs: Identificador del usuario dueño del proceso
    * 
    * Objetivo:
    * - Insertar un nuevo nodo al final de la lista
    * 
    * Descripción:
    * - Crea un nuevo nodo y copia los datos del nodo "ejecucion" en él.
    * - Inserta el nuevo nodo al final de la lista.
    * - Si la lista está vacía, el nuevo nodo se convierte en el primer nodo.
    * - Asigna un PID al nuevo nodo.
    * - Abre el archivo asociado al nuevo nodo.
    */
int contadorPID = 1;
int Insertar(struct PCB **PrimerNodo, struct PCB **Nuevos, char *nombrePrograma, int PBase, int IDUs, FILE *SWAP, int Repetido) {
    struct PCB *nuevoNodo = (struct PCB*)malloc(sizeof(struct PCB));
    // Verifica si se pudo reservar memoria
    if(nuevoNodo == NULL){
        return 0;
    }
    // Copia los datos del nodo "ejecucion" en el nuevo nodo
    nuevoNodo->PID = contadorPID++;
    nuevoNodo->programa = fopen(nombrePrograma, "r");
    // Verifica si se pudo abrir el archivo
    if (nuevoNodo->programa == NULL) {
        free(nuevoNodo);
        return 0;
    }
    // Copia el nombre del archivo
    strcpy(nuevoNodo->fileName, nombrePrograma);
    // Inicializa los registros del CPU

    nuevoNodo->AX = 0;
    nuevoNodo->BX = 0;
    nuevoNodo->CX = 0;
    nuevoNodo->DX = 0;
    nuevoNodo->PC = 0;
    nuevoNodo->KCPU = 0;
    nuevoNodo->UID = IDUs;
    nuevoNodo->P= PBase;
    nuevoNodo->KCPUxU=0;
    strcpy(nuevoNodo->IR, "");
    nuevoNodo->TmpSize = 0;
    nuevoNodo->TMS=TablaTMS;
    // Inserta el nuevo nodo al final de la lista
    
    nuevoNodo->sig = NULL;

    FILE *archivo = fopen(nombrePrograma, "r");
    if (archivo != NULL) {
        char linea[100];
        int lineas = 0;
        while (fgets(linea, sizeof(linea), archivo) != NULL) {
            lineas++;
        }
        fclose(archivo);
        nuevoNodo->TmpSize = (lineas+15) / 16;
    }

    if(nuevoNodo->TmpSize > TMP_SIZE_MAX){
        return 0;
    }

    int marcosLibres=0;
    for(int i=0; i<TMP_SIZE_MAX; i++){
        if(TablaTMS[i].PID == 0){
            marcosLibres++;
        }
    }

    int marcosNecesarios = nuevoNodo->TmpSize;
    if (marcosNecesarios > marcosLibres) {
        InsertarNuevo(Nuevos, nuevoNodo);
        printf("No hay marcos suficientes %d\n", nuevoNodo->PID);
        return 0;
    }
    else if(marcosLibres < marcosNecesarios){
        return 0;
    }

    AsignarMarcos(nuevoNodo, SWAP, nombrePrograma, Repetido);
    
    if (*PrimerNodo == NULL) {
        *PrimerNodo = nuevoNodo;
        return 0;
    }
    // Recorre la lista hasta llegar al último nodo

    struct PCB *Ultimo = *PrimerNodo;
    // Inserta el nuevo nodo al final de la lista
    while (Ultimo->sig != NULL) {
        Ultimo = Ultimo->sig;
    }

    Ultimo->sig = nuevoNodo;
    
    return nuevoNodo->PID;

}

/*
    * ListaInsertarFinal.
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista
    * - ejecucion: Puntero al nodo que se va a insertar
    * 
    * Objetivo:
    * - Insertar un nuevo nodo al final de la lista de procesos terminados
    * 
    * Descripción:
    * - Utiliza la función "InsertarNuevo" para insertar el nodo al final de la lista.
    */
void ListaInsertarFinal(struct PCB **PrimerNodo, struct PCB *ejecucion) {
    
    InsertarNuevo(PrimerNodo, ejecucion);
}

/*
    * KillFinal.
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista de procesos listos
    * - pid: Identificador del proceso a finalizar
    * - terminados: Puntero al puntero del primer nodo de la lista de procesos terminados
    * 
    * Valor de retorno:
    * - 0 si el proceso se finalizó correctamente, -1 si no se encontró el proceso
    * 
    * Objetivo:
    * - Finalizar un proceso y moverlo a la lista de procesos terminados
    * 
    * Descripción:
    * - Busca el proceso con el ID especificado en la lista de procesos listos.
    * - Si encuentra el proceso, lo extrae de la lista de procesos listos y lo inserta en la lista de procesos terminados.
    * - Cierra el archivo asociado al proceso.
    * - Retorna 0 si el proceso se finalizó correctamente, -1 si no se encontró el proceso.
    */
int KillFinal(struct PCB **PrimerNodo, int pid, struct PCB **terminados){
    // Verifica si la lista de procesos listos está vacía
    if (*PrimerNodo == NULL){
        return -1;
    }

    // Verifica si el proceso a finalizar es el primer nodo
    struct PCB *Actual = *PrimerNodo;
    struct PCB *anterior = NULL;

    // Busca el proceso con el ID especificado
    while (Actual != NULL && Actual->PID != pid) {
        anterior = Actual;
        Actual = Actual->sig;
    }

    // Si encuentra el proceso, lo extrae de la lista de procesos listos
    if (Actual != NULL) {
        // Lo extrae de la lista de procesos listos
        if (anterior == NULL) {
            *PrimerNodo = Actual->sig;
        // Si no es el primer nodo
        } else {
            anterior->sig = Actual->sig;
        }
        // Lo inserta en la lista de procesos terminados
        Actual->sig = NULL;
        // Inserta el proceso en la lista de procesos terminados
        InsertarNuevo(terminados, Actual);
        fclose(Actual->programa);
        // Retorna 0 si el proceso se finalizó correctamente
    } else {
        return -1;
    }
    return 0;
}

/*
    * Kill.
    * 
    * Parámetros:
    * - PrimerNodo: Puntero al puntero del primer nodo de la lista de procesos listos
    * - pid: Identificador del proceso a finalizar
    * 
    * Valor de retorno:
    * - 0 si el proceso se finalizó correctamente, -1 si no se encontró el proceso
    * 
    * Objetivo:
    * - Finalizar un proceso
    * 
    * Descripción:
    * - Busca el proceso con el ID especificado en la lista de procesos listos.
    * - Si encuentra el proceso, lo extrae de la lista de procesos listos y libera la memoria del nodo.
    * - Retorna 0 si el proceso se finalizó correctamente, -1 si no se encontró el proceso.
    */
int Kill(struct PCB **PrimerNodo, int pid) {
    // Verifica si la lista de procesos listos está vacía
    if (*PrimerNodo == NULL) {
        return -1;
    }

    // Verifica si el proceso a finalizar es el primer nodo
    if((*PrimerNodo)->PID == pid){
        struct PCB *Actual = *PrimerNodo;
        *PrimerNodo = (*PrimerNodo)->sig;
        free(Actual);
        return 0;
    }

    // Busca el proceso con el ID especificado
    struct PCB *Actual = *PrimerNodo;
    struct PCB *anterior = NULL;

    // Busca el proceso con el ID especificado
    while (Actual != NULL && Actual->PID != pid) {
        anterior = Actual;
        Actual = Actual->sig;
    }

    // Si encuentra el proceso, lo extrae de la lista de procesos listos
    if (Actual != NULL) {
        anterior->sig = Actual->sig;
        free(Actual);
    } else {
        return -1;
    }
        
    return 0;
}
