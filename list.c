/*
 * list.c - implementation of the integer list functions 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"



list_t* lst_new()
{
   list_t *list;
   list = (list_t*) malloc(sizeof(list_t));
   list->first = NULL;
   return list;
}


void lst_destroy(list_t *list)
{
	struct lst_iitem *item, *nextitem;

	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item);
		item = nextitem;
	}
	free(list);
}


void insert_new_task(list_t *list, int prio, char* id)
{
	lst_iitem_t *item;

	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	item->prio = prio;
	item->id = id;
  //TODO:
  //inserir na posição correcta da lista. Aceder à posicão list->prio<int prio> e inserir.
  //criar tambem em ltist_iitem_t suporte para guardar tempos de criação (ver código disponibilizado na aula 1)
	item->next = list->first;
	list->first = item;
}


void update_terminated_task(list_t *list, int prio, time_t endtime)
{
  lst_iitem_t *item;
  item = list->first;
  while (item != NULL)
  {
    if(item->prio == prio)
    {
      /*item->endtime = endtime;*/
      break;
    }
    item = item->next;
  }
  printf("teminated task with pid: %d\n", item->prio);
}


void lst_print(list_t *list)
{
	lst_iitem_t *item;

	printf("task list with start and end time:\n");
	item = list->first;
	/*while(1){  use it only to demonstrate gdb potencial*/
	while (item != NULL){
		/*printf("%d\t%s", item->pid, ctime(&(item->starttime)));
		printf("\t%s", ctime(&(item->endtime)));*/
		item = item->next;
	}
	printf("-- end of list.\n");
}