#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "commandlinereader.h"

#define MAX_ARGS 6

void *monitoring(void *value){
  long testValue = (long) value;
  printf("new thread, here's the test value: %ld\n", testValue);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int numtokens = 0;
  int childPID;
  int status = 0;
  int i = 0;
  int numprocesses = 0;
  pthread_t monitoringThread;
  int rc;
  long test = 90;

  rc = pthread_create(&monitoringThread, NULL, (void *)monitoring, (void *)test);
  printf("RC: %d\n", rc);

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
      break;
      }
    }

    childPID = fork();

    if(childPID >= 0) // fork was successful
    {
      numprocesses++;
      if(childPID == 0) // child process
      {
        childPID = getpid();
        execv(argv[0], argv);
        perror("Error: something wrong with the process.");       
      }
      else numtokens = 0; //Parent process
    }
    else // fork failed
    {
      perror("Error: Fork failed\n");
      return 1;
    }
  }

  for(; i < numprocesses; i++)
  {
    childPID = wait(&status);
    if(childPID <= 0) perror("Error");
    if(WIFEXITED(status))
    {
      printf("pid: %d\tstatus: %d\n", childPID, WEXITSTATUS(status));
    }
  }

  return 0;
}
