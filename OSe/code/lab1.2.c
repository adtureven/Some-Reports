#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>


int variable = 0;

void* thread_func1(void* arg) {
    printf("thread1 create successfully!\n");
    for(int i=0;i<10000;i++){
    	variable+=1;
	}
    return NULL;
}

void* thread_func2(void* arg) {
    printf("thread2 create successfully!\n");
    for(int i=0;i<10000;i++){
    	variable-=1;
	}
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, thread_func1, NULL);
    pthread_create(&thread2, NULL, thread_func2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("variable = %d\n", variable);

    return 0;
}
