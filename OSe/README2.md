# **进程通信与内存管理**

## **进程的软中断通信**

1.man命令

problem：
1.直接使用man命令，出现错误`no manual entry for kill in section 2` ![image-20231018182404393](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018182404393.png)

发现是因为未下载和安装man的安装包；调用`yum install man`和`yum install man-pages`命令进行man的下载和安装。

![image-20231018183050976](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183050976.png)

![image-20231018183120033](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183120033.png)

之后显示已经complete；在调用man命令就无误了；

2.查阅man 后面的数字的含义，发现为2时是题目所需的系统调用。而sleep在section2没有说明，在section3中才有。

![image-20231018184301355](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018184301355.png)

![image-20231018183451816](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183451816.png)



**运行结果如下：**

fork ：

![image-20231018183754320](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183754320.png)

![image-20231018183650792](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183650792.png)

kill ：

![image-20231018183858023](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183858023.png)

![image-20231018183830511](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183830511.png)

signal：

![image-20231018183938701](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183938701.png)

![image-20231018183923732](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018183923732.png)

sleep：

![image-20231018184123281](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018184123281.png)

![image-20231018184108647](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018184108647.png)

exit： 

![image-20231018184040764](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018184040764.png)

![image-20231018184027165](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231018184027165.png)



2.软中断通信

problem:

1. 在父进程中调用kill后向子进程发出信号后未终止，导致父进程一直卡在wait处；

![image-20231023151359895](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023151359895.png)

将pause处连带循环一起删去即可；

2. 没过5秒，键盘没有产生中断，子进程直接显示被kill；

子进程未收到信号就终止了

S:在子进程处也设置flag值即可，收到信号才能向后运行。

3. 使用Ctrl c或者Ctrl \ 产生中断信号时，父进程直接终止，无子进程输出；

   ![image-20231023153823515](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023153823515.png)

   将delete和quit中断信号放在main函数中，移出父进程。

   4.使用time头函数记录等待时间,start记录开始，end记录收到任一信号时的时间。



**运行结果：**

![image-20231023160812845](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023160812845.png)

**理论分析：**

首先输出了一个数字，表示父进程运行的时间。这个数字是由于在程序中设置了一个定时器，当定时器超时时会触发一个信号，从而让父进程退出。

接着输出了两个数字，分别为16和17。这两个数字表示父进程向子进程发送的信号，其中16表示SIGUSR1信号，17表示SIGUSR2信号。当子进程收到这些信号时，会触发相应的信号处理函数，从而让子进程退出。

然后输出了两行信息，分别表示两个子进程被父进程杀死了。这是由于子进程在收到相应的信号后，会触发相应的信号处理函数，从而让子进程退出。

最后输出了一行信息，表示父进程被杀死了。这是由于父进程在定时器超时后，会触发相应的信号处理函数，从而让父进程退出。

程序的原理是通过信号传递来控制子进程的运行和退出，其中父进程通过kill函数向子进程发送信号，子进程通过signal函数来注册信号处理函数。当子进程收到相应的信号时，会触发相应的信号处理函数，从而让子进程退出。





*源代码*

```c
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

```













## **进程的管道通信**

problem：

1. 管道write函数，第二个参数不能直接使用单字符；

   ![image-20231023180120157](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023180120157.png)

使用字符指针即可；



**运行结果：**

有锁：

![image-20231023181109503](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023181109503.png)

无锁：

![image-20231023181321015](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231023181321015.png)

**理论分析：**

以上程序加锁的作用是保证两个子进程写入管道时不会互相干扰，从而保证数据的完整性和正确性。如果不加锁，两个子进程同时写入管道时可能会出现数据混乱或者丢失的情况。

具体来说，加锁的方式是使用lockf函数对管道进行互斥锁操作。lockf函数的第一个参数是文件描述符，第二个参数是锁定方式，0表示解锁，1表示锁定，第三个参数是锁定的字节数。在本程序中，我们对写入管道的代码进行了加锁，即在写入之前先锁定管道，写完之后再解锁管道。

如果不加锁，程序输出的结果可能会出现以下两种情况：

1. 数据混乱：由于两个子进程同时写入管道，数据可能会交错出现，从而导致输出结果的顺序与预期不符。

2. 数据丢失：由于两个子进程同时写入管道，数据可能会被覆盖或者丢失，从而导致输出结果不完整或者错误。

因此，加锁是一种保证数据正确性的重要手段，特别是在多进程或者多线程的环境中，更需要注意加锁的问题。





*源代码*

```c
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
```









## **页面置换**



**FIFO random：**

![image-20231030110049201](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231030110049201.png)





**FIFO manual input：**

![image-20231030110312307](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231030110312307.png)





**LRU random：**

![image-20231030110405274](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231030110405274.png)





**LRU manual input：**

![image-20231030110517068](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231030110517068.png)



**FIFO的Belady现象：**

增加物理块的数量，缺页率有可能会增长；

以下是对比（页表数为6，引用序列都相同，数量为12）：

1 2 3 4 1 2 5 1 2 3 4 5

Frames=3 :

![image-20231030122006808](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231030122006808.png)



Frames=4:

![image-20231030122252334](C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231030122252334.png)



在FIFO算法下，随着页面帧数的增加，缺页率却可能会增加，而不是减少。这与直觉相反，因为通常认为增加内存帧数应该降低缺页率。这一现象的原因在于FIFO算法的工作方式。

此时，Belady现象出现了。尽管页面帧数增加了，但缺页率却没有减少，而是增加了。这是因为FIFO算法替换的不是"最近使用"最少的页面，而是"最早进入"的页面，而这些可能是在更大页面帧数下被淘汰的页面。因此，增加页面帧数可能会导致更多页面被淘汰，从而增加了缺页率。

这种情况表明，FIFO算法不是一个优秀的页面置换算法，特别是当系统中页面帧数较多时，它容易导致Bleady现象。





*源代码：*

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_FRAMES 3     //物理块的数量
#define MAX_PAGES 8		 //页表的数量
#define MAX_STRING 20    //引用序列的长度

int frames[MAX_FRAMES];
int pageReference[MAX_STRING];

//initialize
void initializeFrames() {
    for (int i = 0; i < MAX_FRAMES; i++) {
        frames[i] = -1;
    }
}

void printFrames() {
    for (int i = 0; i < MAX_FRAMES; i++) {
        if (frames[i] == -1) {
            printf("X ");
        } else {
            printf("%d ", frames[i]);
        }
    }
    printf("\n");
}

int isPageInFrames(int page) {
    for (int i = 0; i < MAX_FRAMES; i++) {
        if (frames[i] == page) {
            return i;
        }
    }
    return -1;
}

void FIFO() {
    int pageFaults = 0;
    int currentIndex = 0;

    for (int i = 0; i < MAX_STRING; i++) {
        int page = pageReference[i];
        printf("Page %d: ", page);

        if (isPageInFrames(page)==-1) {
            frames[currentIndex] = page;
            currentIndex = (currentIndex + 1) % MAX_FRAMES;
            pageFaults++;
        }
        printFrames();
    }

    printf("FIFO Page Faults: %d\n", pageFaults);
    printf("FIFO Page Fault Rate: %.2f%%\n", (float)pageFaults / MAX_STRING * 100);
}

void LRU() {
    int pageFaults = MAX_FRAMES;
    int Counter[MAX_FRAMES] = {0};
	
	for(int i = 0; i < MAX_FRAMES; i++) {
		Counter[i]=MAX_FRAMES-i;
		frames[i]=pageReference[i];
		printf("Page %d: ",frames[i]);
		printFrames();
	}
    for (int i = MAX_FRAMES; i < MAX_STRING; i++) {
        int page = pageReference[i];
        printf("Page %d: ",page);

        if (isPageInFrames(page)==-1) {
            int replaceIndex = 0;
            
            for (int j = 1; j < MAX_FRAMES; j++) {
                if (Counter[j] > Counter[replaceIndex]) {
                    replaceIndex = j;
                }
            }

            frames[replaceIndex] = page;
            pageFaults++;
        }

        Counter[isPageInFrames(page)] = 0;
        for (int j = 0; j < MAX_FRAMES; j++) {
            if (frames[j] != -1) {
                Counter[j]++;
            }
        }
        
        printFrames();
    }

    printf("LRU Page Faults: %d\n", pageFaults);
    printf("LRU Page Fault Rate: %.2f%%\n", (float)pageFaults / MAX_STRING * 100);
}


int main() {
    initializeFrames();
    int choice, i;

    //select algorithm
    printf("Select the page replacement algorithm:\n");
    printf("1. FIFO\n");
    printf("2. LRU\n");
    scanf("%d", &choice);
	//invalid input
    if (choice != 1 && choice != 2) {
        printf("Invalid choice.\n");
        return 1;
    }

    //select way
    printf("Select how to generate page references:\n");
    printf("1. Random\n");
    printf("2. Manual Input\n");
    scanf("%d", &i);

    if (i != 1 && i != 2) {
        printf("Invalid choice.\n");
        return 1;
    }

    if (i == 1) {
        srand(time(NULL));
        printf("Generated Page References: ");
        for (i = 0; i < MAX_STRING; i++) {
            pageReference[i] = rand() % MAX_PAGES;
            printf("%d ", pageReference[i]);
        }
        printf("\n");
    } else {
        printf("Enter Page References (separate with spaces): ");
        for (i = 0; i < MAX_STRING; i++) {
            scanf("%d", &pageReference[i]);
        }
    }

    if (choice == 1) {
        printf("\nFIFO Algorithm:\n");
        FIFO();
    } else {
        printf("\nLRU Algorithm:\n");
        LRU();
    }

    return 0;
}

```





