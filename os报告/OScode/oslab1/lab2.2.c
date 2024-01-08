#include <unistd.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <wait.h>
 
int main( ) {
	pid_t pid1,pid2;
 	int fd[2]; 
	char InPipe[5000];
	char *c1="1", *c2="2"; 
	pipe(fd);
	while((pid1 = fork( )) == -1);
	if(pid1 == 0) { 
		//lockf(fd[1],1,0);
		for(int i=0;i<2000;++i){
			write(fd[1],c1,1);
		}
		sleep(5);
		//lockf(fd[1],0,0);
		exit(0);
	} 
	else { 
		while((pid2 = fork()) == -1);
		if(pid2 == 0) { 
			//lockf(fd[1],1,0); 
			for(int i=0;i<2000;++i){
				write(fd[1],c2,1);
			}
			sleep(5); 
			//lockf(fd[1],0,0); 
			exit(0); 
		} 
		else { 
			wait(0);
			wait(0);
			read(fd[0],InPipe,sizeof(InPipe));
			InPipe[4000]='\0';
			printf("%s\n",InPipe);
			exit(0);
		} 
	}
	return 0;
}