/*
// Command line reader (header file), version 2
// Sistemas Operativos, DEI/IST/ULisboa 2015-16
*/
 
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

/* 
Reads up to 'vectorSize' space-separated arguments from the standard input
and saves them in the entries of the 'argVector' argument.
This function returns once enough arguments are read or the end of the line 
is reached

Arguments: 
 'argVector' should be a vector of char* previously allocated with
 as many entries as 'vectorSize'
 'vectorSize' is the size of the above vector. A vector of size N allows up to 
 N-1 arguments to be read; the entry after the last argument is set to NULL.

Return value:
 The number of arguments that were read, or -1 if some error occurred.
*/

int readLineArguments(char **argVector, int vectorSize)
{
  int childPID;
  int status = 0;

  int numtokens = 0;
  char *s = " \n\t";

  char *str = NULL;
  size_t size = 0;
  int i;

  char *token;

  if (argVector == NULL || vectorSize == 0)
    return 0;

  if (getline(&str, &size, stdin) < 0) {
    return -1;
  }

   
  /* get the first token */
  token = strtok(str, s);
  if(!strcmp(token, "exit") || !strcmp(token, "EXIT") || !strcmp(token, "Exit")) {
    printf("process %d exiting...\n", getpid());
    exit(EXIT_FAILURE);
  }

  childPID = fork();

    if(childPID >= 0) // fork was successful
    {
      if(childPID == 0) // child process
      {
        
        while( numtokens < vectorSize-1 && token != NULL ) {
          argVector[numtokens] = token;
          numtokens ++;
          token = strtok(NULL, s);
        }

        argVector[numtokens+1] = NULL;
        execv(argVector[0], argVector);       
      }
      else //Parent process
      {
        printf("parent process %d is waiting...\n", getpid());
        wait(&status);
        printf("\n Parent process %d waited for child to terminate with status %d\n", getpid(),status);
      }
    }
    else // fork failed
    {
      printf("\n Fork failed\n");
      return 1;
    }

  for (i = numtokens; i<vectorSize; i++) {
    //printf("%c\n", argVector[i] );
    argVector[i] = NULL;
  }
   
  return numtokens;
}

