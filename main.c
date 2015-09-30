/*
 * main.c -- main program function
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "commandlinereader.h"

 #define MAX_ARGS 6


/*
 * main program
 */

int main(int argc, char *argv[])
{
  list_t *list;

  printf("<<BEGIN>>\n");

  /* initialize list */
  list = lst_new();

  int result = 0;

  result = readLineArguments(argv, MAX_ARGS);

  printf("%d\n", result );

  /* insert several elements into list 
  insert_new_task(list, 1, "id1");
  sleep(1);
  insert_new_task(list, 2, "id2");
  sleep(2);
  insert_new_task(list, 3, "id3");
  sleep(2);
  insert_new_task(list, 0, "id4");
  sleep(1);
  insert_new_task(list, 5, "id5");*/

  /* print list 'list' 
  lst_print(list);*/

  /* remove elements from list /
  update_terminated_task(list, 1, time(NULL));
  sleep(1);
  update_terminated_task(list, 2, time(NULL));
  sleep(2);
  update_terminated_task(list, 5, time(NULL));*/

  /* print list /*/

  /* clean up list */
  lst_destroy(list);

  printf("<<END>>\n");

  return 0;
}
