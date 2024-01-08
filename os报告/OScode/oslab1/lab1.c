#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main(){
	pid_t pid;
	pid=fork();
	
	if(pid<0){
		fprintf(stderr,"Fork failed");
		return 1;
	}
	else if(pid==0){
		printf("child process1 PID: %d\n",getpid());
		execl("/usr/local/src/test/system_call","system_call",NULL);
		printf("child process PID: %d\n",getpid());
		perror("execl");
		exit(1);
	}
	else{
		printf("parent process PID: %d\n",getpid());
		wait(NULL);
	}
	
	return 0;
}