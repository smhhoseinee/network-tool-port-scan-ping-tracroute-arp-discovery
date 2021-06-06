#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
  
// A normal C function that is executed as a thread 
// when its name is specified in pthread_create()
void *myThreadFun(void *vargp)
{
    printf("Printing GeeksQuiz from Thread \n");
    sleep(1);
    return NULL;
}

void *scan_port(char *server_addr_str,int server_port,int sock){

	return 0;
}


int main()
{
	pthread_t thread_id;
	printf("Before Thread\n");
	//    pthread_create(&thread_id, NULL, myThreadFun, NULL);
	//    pthread_join(thread_id, NULL);
	//    printf("After Thread\n");
	//    exit(0);

	pid_t pid;
	pid = fork();
	if (pid == 0){
		fork();
		pthread_create(&thread_id, NULL, myThreadFun, NULL );

		pthread_join(thread_id, NULL);
	}
	fork();

	printf("process finished\n");


}
