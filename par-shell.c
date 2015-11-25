/*
 * Par-shell - exercicio 2, version 1
 * Sistemas Operativos, DEI/IST/ULisboa 2015-16
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#include "commandlinereader.h"
#include "list.h"


#define EXIT_COMMAND "exit"

#define MAXARGS 7
#define MAXPAR 4
#define BUFFER_SIZE 100

#define STDIN 1
#define STDOUT 2

#define RDWR_PERMISSIONS 0644

#define PIPE_BUF_SZ 100

#define MYFIFO "/afs/ist.utl.pt/users/3/4/ist158534/Documents/par_shell_pipes/par-shell-in"


int num_children = 0;
int flag_exit = 0; /* do not exit */

list_t *proc_data;

pthread_mutex_t data_ctrl;
pthread_cond_t concurrency = PTHREAD_COND_INITIALIZER;
pthread_cond_t monitor_cycle = PTHREAD_COND_INITIALIZER;

/*sem_t sem_main, sem_monitoring;*/

/*-------------- Tratamento de Ficheiros------------------------------*/
FILE *file; 
int iteration = 0, totaltime = 0;
char buffer1 [100];
char buffer2 [100];
char buffer3 [100];
int iterationNumber, totalIterationTime;   
int childtotal_time ;

char pidFilename[25];
 

/* 
+-----------------------------------------------------------------------*/
void mutex_lock(void) {
  if(pthread_mutex_lock(&data_ctrl) != 0)
  {
    fprintf(stderr, "Error in pthread_mutex_lock()\n");
    exit(EXIT_FAILURE);
  }
}


/* 
+-----------------------------------------------------------------------*/
void mutex_unlock(void) {
  if(pthread_mutex_unlock(&data_ctrl) != 0)
  {
    fprintf(stderr, "Error in pthread_mutex_unlock()\n");
    exit(EXIT_FAILURE);
  }
}


/* 
+-----------------------------------------------------------------------*/
void *tarefa_monitora(void *arg_ptr) {
  int status, childpid;
  time_t end_time;

lst_iitem_t *proc = malloc (sizeof(lst_iitem_t*));
  printf(" *** Tarefa monitora activa.\n");

  while(1) {
    /*sem_wait(&sem_monitoring);*/
    mutex_lock();
    while(num_children == 0 && flag_exit == 0){
      pthread_cond_wait(&monitor_cycle, &data_ctrl);
    }
    if(num_children == 0) {
      if(flag_exit == 1) {
        mutex_unlock();
        printf(" *** Tarefa monitora terminou.\n");
        pthread_exit(NULL);
      }
    }
    mutex_unlock();
    pthread_cond_signal(&concurrency);

    /* wait for a child process */

    childpid = wait(&status);
    mutex_unlock();
    if (childpid == -1) {
      perror("Error waiting for child");
      exit(EXIT_FAILURE);
    }
    end_time = time(NULL);
    mutex_lock();
    num_children --;
    proc = update_terminated_process(proc_data, childpid, end_time, status);
    if(proc)
    {
      childtotal_time = (int)(proc->endtime - proc->starttime);
      totalIterationTime += childtotal_time;
      fprintf(file,"iteracao %d\n", iteration);   
      fprintf(file,"pid: %d execution time: %d s\n", proc->pid, childtotal_time); 
      fprintf(file,"total execution time: %d s\n", totalIterationTime);
      iteration++;
    }
    mutex_unlock();
    pthread_cond_signal(&concurrency);
  }
}


/* 
+-----------------------------------------------------------------------*/
int main (int argc, char** argv) {
  pthread_t tid;
  char buffer[BUFFER_SIZE];
  int numargs;
  char *args[MAXARGS];
  time_t start_time;
  int pid;
  int childFd;
  int pfd[2];  /* Pipe file descriptors */
  int bytesRead = 0;
  int fd;

  char * myfifo = MYFIFO;

  mkfifo("/tmp/par-shell-indf", 0666);

  if ((file = fopen("log.txt","r")) == NULL)
  {
  	file = fopen("log.txt", "a");
  	fclose(file);
  }
  
  file = fopen("log.txt","a+");

  while (fgets(buffer1, sizeof(buffer1), file) != NULL)
  {
		fgets(buffer2, (int)sizeof(buffer2), file);
		fgets(buffer3, (int)sizeof(buffer3), file);
		sscanf(buffer3, "%*s %*s %*s %d %*s", &totalIterationTime);
    iteration++;
  }
   
  /*sem_init(&sem_main, 0, MAXPAR);
  sem_init(&sem_monitoring, 0, 0);*/


  /* criar estrutura de dados de monitorização */
  proc_data = lst_new();

  /* criar mutex */
  if(pthread_mutex_init(&data_ctrl, NULL) != 0) {
    fprintf(stderr, "Error creating mutex.\n");
    exit(EXIT_FAILURE);
  }

  /* criar thread */
  if (pthread_create(&tid, NULL, tarefa_monitora, NULL) != 0) {
    fprintf(stderr, "Error creating thread.\n");
    exit(EXIT_FAILURE);
  }


  while(1) {
    
    fd = open("/tmp/par-shell-in", O_RDONLY);
    read(fd, buffer, BUFFER_SIZE);
    printf("Received: %s", buffer);
    close(fd);
    

    //printf("Insert your command: ");
    numargs = readLineArguments(args, MAXARGS, buffer, BUFFER_SIZE);

    if (numargs < 0) {
      printf("Error in readLineArguments()\n");
      continue;
    }
    if (numargs == 0) /* empty line; read another one */
      continue;

    if (strcmp(args[0], EXIT_COMMAND) == 0) {
      printf("Ending...\n");
      /*sem_post(&sem_monitoring);*/
      break;
    }

    

    /* process a command */
    start_time = time(NULL);
    while(num_children >= MAXPAR){
      pthread_cond_wait(&concurrency, &data_ctrl);  
    }
    
    /*sem_wait(&sem_main);*/
    pid = fork();
    pthread_cond_signal(&monitor_cycle);
    if (pid == -1) {
      perror("Failed to create new process.");
      exit(EXIT_FAILURE);
    }

    if (pid > 0) {  /* parent */
      mutex_lock();
      num_children ++;
      insert_new_process(proc_data, pid, start_time);
      pthread_cond_signal(&monitor_cycle);
      /*sem_post(&sem_monitoring);*/
      mutex_unlock();
    }
    else if (pid == 0) {  /* child */

     
      sprintf(pidFilename, "par-shell-out-%d.txt", (int)getpid());

      childFd = open(pidFilename, O_CREAT|O_APPEND|O_RDWR,  RDWR_PERMISSIONS);

      close(STDIN);

      dup(childFd);   // stdout

      if (execv(args[0], args) == -1) {
        perror("Could not run child program. Child will exit.");
        exit(EXIT_FAILURE);
      }
      close(childFd);
    }
    mutex_unlock();
  }

  /* received command exit */

  /* request the monitoring thread to end */
  mutex_lock();
  flag_exit = 1;
  pthread_cond_signal(&monitor_cycle);
  mutex_unlock();
  
  /* wait for thread to end */
  if(pthread_join(tid, NULL) != 0) {
    fprintf(stderr, "Error joining thread.\n");
    exit(EXIT_FAILURE);
  }


  lst_print(proc_data);

  /* clean up and exit */
  pthread_mutex_destroy(&data_ctrl);
  lst_destroy(proc_data);
  pthread_cond_destroy(&monitor_cycle);
  pthread_cond_destroy(&concurrency);

  fclose(file);
  return 0; /* ok */
}

