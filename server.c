//server
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<sys/sem.h>

#define KEY 0x1111

 struct data{
	int service;
	char stg1[100];  //service 1
	char stg2[100];
	char stg3[100];
	char stg4[100];
	double matrix[5];  //service 2
	double inx;
	int factorial;  //service 3
	int shared; 
	int client;     
}i_data; 	//input_data = i_data

struct q_data {
	struct data que[100];
	int numb;
};

void my_handler(){}

union semun {
    int value;
    struct semid_ds *buf;
    unsigned short  *arr;
};

struct sembuf p = { 0, -1, SEM_UNDO};
struct sembuf v = { 0, +1, SEM_UNDO};

void handle_sigint() 
{ 
  char * command = "ipcrm -a";
  system(command);
  printf("\nRequest Queue is cleared.\nServer is Exiting.\n"); 
  exit(0);
} 

int main()
{
  	char mtrx[5][100]; //mat = mtrx
  	char fact[100];
	char buffer[100];
  	char shared_id[100];
  	char clint_id[100];
    signal(SIGUSR1,my_handler);
	printf("---- Server program has started ----- \n");

	//Creating Shared Memory for que
	key_t key = ftok("request.txt",10);
	if(key<0){
		perror("errorKeyQueueServer: ");
		exit(0);
	}
	int shmid= shmget(key,sizeof(struct q_data),IPC_CREAT | 0666);
	if(shmid<0){
		perror("errorShmServer: ");
		exit(0);
	} 
	struct q_data* queue = (struct q_data*)shmat(shmid,NULL,0);
	if(queue==(void*)-1){
		perror("errorQueueServer: ");
		exit(0);
	}

	//semaphore implementation
    int id = semget(KEY, 1, 0666 | IPC_CREAT);
    if(id < 0) {
        perror("semget"); 
        exit(11);
    }
    union semun u;
    u.value = 1;
    if(semctl(id, 0, SETVAL, u) < 0){
        perror("semctl"); 
        exit(12);
    }

	//shared memory variables initialize
	//queue->numb = -1;
	int front,back=0;
	printf("\nReady for Operations!\n");

while(1){
	
    signal(SIGINT, handle_sigint); 
	//cs start
    if(semop(id, &p, 1) < 0){
    	perror("semop p"); exit(13); 
    }
	
	front = queue->numb;

    if(semop(id, &v, 1) < 0){
        perror("semop p"); exit(14); 
    }
	//cs end

	if(front == -1)		
		exit(0);
	else if(front >= back){
	    //cs start
        if(semop(id, &p, 1) < 0){	
        	perror("semop p"); exit(13); 
        }
				i_data = queue->que[back];
        
        if(semop(id, &v, 1) < 0){   
        	perror("semop p"); exit(14); 
		}
		//cs end

		int pid = fork();
		printf("%d\n",pid);
		int ch = i_data.service;
		
		if(pid == 0){
            snprintf(shared_id,100,"%d",i_data.shared);
            snprintf(clint_id,100,"%d",i_data.client);
			
			if(ch == 1)	{
				printf("Entered 1st service\n");
				execl("./service1", "./service1", i_data.stg1,i_data.stg2,i_data.stg3,i_data.stg4,shared_id,clint_id, NULL);
				break;
				exit(0);
			}
		    else if(ch == 2) {
				printf("Entered 2nd service\n");
				printf("p=%lf\n",i_data.inx);
            	for(int i=0;i<5;i++)
                		snprintf(mtrx[i],100,"%lf",i_data.matrix[i]);
		snprintf(buffer,100,"%lf",i_data.inx);	
				execl("./service2", "./service2", mtrx[0],mtrx[1],mtrx[2],mtrx[3],mtrx[4],buffer,shared_id,clint_id, NULL);
				break;
				exit(0);
			}
			else if(ch == 3) {
				printf("Entered 3rd service\n");
            	snprintf(fact,100,"%d",i_data.factorial);
				execl("./service3", "./service3",fact,shared_id,clint_id, NULL);
				break;
				exit(0); 
			}
      	}
		back++;
	}
	else {
		back= 0;
		//cs
        if(semop(id, &p, 1) < 0){
          perror("semop p"); 
          exit(13); 
      	}
				queue->numb = -1;
        
        if(semop(id, &v, 1) < 0){
          perror("semop p"); 
          exit(14); 
      	}
		//cs end
	}
	sleep(1);
  }	
}
