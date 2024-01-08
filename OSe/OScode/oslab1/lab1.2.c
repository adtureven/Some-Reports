#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/syscall.h>

void* func1(void* arg) {
    printf("thread1 create successfully!\n");
    printf("thread1 tid = %d , pid = %d\n",syscall(SYS_gettid),getpid());
       sleep(1);
    execl("/usr/local/src/test/system_call","system_call",NULL);   
      printf("thread1 systemcall return\n");
       perror("execl");
         exit(1);
    return NULL;
}

void* func2(void* arg) {
    printf("thread2 create successfully!\n");
    printf("thread2 tid = %d , pid = %d\n",syscall(SYS_gettid),getpid());
    execl("/usr/local/src/test/system_call","system_call",NULL);
    printf("thread2 systemcall return\n");
       perror("execl");
    exit(1);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, func1, NULL);
    pthread_create(&thread2, NULL, func2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
