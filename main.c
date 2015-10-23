#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "list.h"
#include "commandlinereader.h"

#define MAX_ARGS 6


list_t *list;
int exit_flag = 0;
int numprocesses = 0;
int status = 0;
pthread_mutex_t mutexChilds;

void *monitoring(){
  while(1){
    pthread_mutex_lock (&mutexChilds);
    if(numprocesses != 0)
    {
      pthread_mutex_unlock (&mutexChilds);
      long child = wait(&status);
      pthread_mutex_lock (&mutexChilds);
      update_terminated_process(list, child, time(NULL));
      numprocesses--;
      pthread_mutex_unlock (&mutexChilds);
    }
    else {
      if(exit_flag == 1)//se nao há - houve exit?
      {
        pthread_mutex_unlock (&mutexChilds);
        pthread_exit(NULL);
      }
      else
      {
        pthread_mutex_unlock (&mutexChilds);
        sleep(1); //se nao há e nao houve exit, sleep
      }
    }
  }
}

int main(int argc, char *argv[])
{
  pthread_t monitoringThread;

  int numtokens = 0;
  int childPID;
  int i = 0;

  list = lst_new();

  pthread_mutex_init(&mutexChilds, NULL);
  pthread_create(&monitoringThread, NULL, (void *)monitoring, NULL);


  while(1){
    numtokens = readLineArguments(argv, MAX_ARGS);
    if(numtokens == 0 || numtokens == -1){
      perror("Error: insufficient arguments");
      continue;
    }
    else {
      if(!strcmp(argv[0], "exit"))
      {
      printf("process %d exiting...\n", getpid());
      exit_flag = 1;
      break;
      }
    }
    childPID = fork();

    if(childPID >= 0) // fork was successful
    {
      pthread_mutex_lock (&mutexChilds);
      numprocesses++;
      pthread_mutex_unlock (&mutexChilds);
      if(childPID == 0) // child process
      {
        childPID = getpid();
        execv(argv[0], argv);
        perror("Error: something wrong with the process.");       
      }
      else{ //parent process
        pthread_mutex_lock (&mutexChilds);
        insert_new_process(list, childPID, time(NULL));
        pthread_mutex_unlock (&mutexChilds);
        numtokens = 0; //Parent process
      }
    }
    else // fork failed
    {
      perror("Error: Fork failed\n");
      return 1;
    }
  }

  pthread_join(monitoringThread, NULL); //added

  for(; i < numprocesses; i++)
  {
    childPID = wait(&status);
    if(childPID <= 0) perror("Error");
    if(WIFEXITED(status))
    {
      printf("pid: %d\tstatus: %d\n", childPID, WEXITSTATUS(status));
    }
  }
  lst_print(list);
  lst_destroy(list);

  return 0;
}
