#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define EXIT "exit"
#define STATS "stats"

#define BUF_SIZE 100

#define MYFIFO "/afs/ist.utl.pt/users/3/4/ist158534/Documents/par_shell_pipes/par-shell-in"

int main(){
	char input[BUF_SIZE];
	int pfd[2];  /* Pipe file descriptors */
	int fd;

	//faz open do pipe

	memset(input, '\0', sizeof(char)*BUF_SIZE);

	while(1){
		fgets(input, BUF_SIZE, stdin);

		if(strcmp(input, EXIT) == 0){
			//faz exit
			puts("faz exit");
		}
		else {
			if(strcmp(input, STATS) == 0){
				//faz stats
				puts("faz stats");
			}
			else{ //manda input para o pipe
        fd = open("/tmp/par-shell-in", O_WRONLY);
    			write(fd, input, BUF_SIZE);
    			close(fd);
          memset(input, '\0', sizeof(char)*BUF_SIZE);
			}
		}
	}
}
