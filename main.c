/*
 * main.c -- main program function
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "commandlinereader.h"

#define MAX_ARGS 6
#define ARRAY_LENGHT 1000


/*
 * main program
 */

int main(int argc, char *argv[])
{
  list_t *list;

  printf("<<BEGIN>>\n");

  /* initialize list */
  list = lst_new();

  //int result = 0; descomentar isto depos
  char str[ARRAY_LENGHT];

  memset(str, '\0', sizeof(char)*ARRAY_LENGHT);

  while(1){
    printf("par_shell > ");
    /*TODO:
    - ao sair, imprimir a lista com os pids e os numtokens dos filhos;
    */
    //result = readLineArguments(argv, MAX_ARGS);

    readLineArguments(argv, MAX_ARGS);

  }
    /* print list */
    //lst_print(list);

    /* clean up list */
    lst_destroy(list);

    printf("<<END>>\n");
  return 0;
}
