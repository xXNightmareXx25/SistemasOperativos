/*
* compilar con gcc -o enc1 -lncurses ejnc1.c 
* ----------
* http://bulma.net/impresion.phtml?nIdNoticia=2004
*/

#include <ncurses.h>

int main() {

  initscr();			// es para entrar en modo ncurses
  printw("Hola Mundo!!!");	// imprimir en la "ventana"*. Es importante sustituir printw, scanfw, etc. por printf, scanf, etc.
  refresh();			// Muestra la pantalla. Podemos hacer varios printw y al final un solo refresh para actualizar la pantalla. 
				// Si no hacemos el refresh quizás no saldrá impreso por pantalla.
  getch();			// Espera una sola pulsación de una tecla. Devuelve el código ASCII de la tecla pulsada como un entero. Fijaros que no espera el "Enter" final de línea
  endwin();			// Termina el modo de ncurses. Si no , quedará el terminal medio desconfigurado. Para solucionarlo podríamos hacer reset desde la misma consola.
  return 0;
}
