#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	pid_t pid=getpid();
	printf("System call PID = %d\n",pid);
	return 0;
}