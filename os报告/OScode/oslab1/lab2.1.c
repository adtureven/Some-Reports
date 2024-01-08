#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

int flag=0,flag1=0,flag2=0;
time_t end ;
void inter_handler() {
    flag = 1;
	end=time(NULL);
}

void c1_handler() {
    flag1 = 1;
}

void c2_handler() {
    flag2 = 1;
}

void waiting() {
    while(!flag){};
}

void waiting1() {
    while(!flag1){};
}
void waiting2() {
    while(!flag2){};
}


int main() {
    pid_t pid1=-1, pid2=-1;
    while (pid1 == -1) pid1=fork();
	signal(SIGINT,inter_handler);
	signal(SIGQUIT,inter_handler);
    if (pid1 > 0) {
        while (pid2 == -1) pid2=fork();
        if (pid2 > 0) {
            // Parent process
			signal(SIGALRM,inter_handler);
			
			time_t start = time(NULL);
			alarm(5);
			waiting();
			printf("%d stop test\n\n",end-start);
           	kill(pid1, 16);
			printf("16 stop test\n");
           	kill(pid2, 17);
			printf("17 stop test\n\n");
            wait(NULL);
            wait(NULL);
            printf("Parent process is killed!!\n");
        } else {
            // Child process 2
        	signal(17, c2_handler);
			waiting2();
            printf("Child process2 is killed by parent!!\n");
            return 0;
        }
    } else {
        // Child process 1
        signal(16, c1_handler);
		waiting1();
        printf("Child process1 is killed by parent!!\n");
        return 0;
    }
    return 0;
}
