//client
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/sem.h>
#include<string.h>

#define KEY 0x1111



 struct data{
	int service;
	char stg1[100];  //service 1 //string = stg
	char stg2[100];
	char stg3[100];
	char stg4[100];
	double matrix[5]; 
        double inx;	//service 2
	int factorial;  //service 3
	int shared; // shared_mem_id = shared
	int client; // client_id = client
}; 	

struct q_data { //data_queue = q_data
	struct data que[100]; // queue = que
	int numb; // num = numb
};

struct answers{ // ANS = answers
	int ans; // answer = ans
	int error; // err = error
	char answer1[100];
	char answer2[100];
	char answer3[100];
	char answer4[100];
};

void my_handler(){}

union semun {
    int value; // Val = value
    struct semid_ds *buf;
    unsigned short  *arr; // array = arr
};

struct sembuf p = { 0, -1, SEM_UNDO};
struct sembuf v = { 0, +1, SEM_UNDO};

int main()
{
	int pid = getpid();
	signal(SIGUSR1,my_handler);
	
	//Creatring Shared Memory for que
	key_t KFQ = ftok("request.txt",10); //keyForQueue = KFQ
	if(KFQ<0){
		perror("errorKeyQueueClient: ");
		exit(0);
	}
	
	int shmFQ = shmget(KFQ,sizeof(struct q_data),IPC_CREAT | 0666); // shmForQueue = shmFQ
	if(shmFQ<0){
		perror("errorShmClient ");
		exit(0);
	} 
	
	struct q_data* queue = (struct q_data*)shmat(shmFQ,NULL,0); //q = queue
	if(queue==(void*)-1){
		perror("errorQueueClient: ");
		exit(0);
	}

    //Creating shared memory for service to client 
	key_t key_uno = ftok("answer.txt", pid); //key1 = key_uno
    if(key_uno==-1){
      perror("error0:");
      exit(1);
    }
    
    int shmid = shmget(key_uno, sizeof(struct answers), IPC_CREAT | 0666);
    if(shmid<0){
      perror("error1:");
      exit(1);
    }
    
    struct answers * myans;
	myans =(struct answers *)shmat(shmid,NULL,0);
    if(myans == (void *) -1){
      perror("error2:");
      exit(1);
    }
	
    //System semaphore implementation
    int id = semget(KEY, 1, 0666 | IPC_CREAT);
    if(id < 0){
        perror("semget"); 
        exit(11);
    }
    union semun u;
    u.value = 1;
    if(semctl(id, 0, SETVAL, u) < 0){
        perror("semctl"); 
        exit(12);
    }

	//taking input of service choice and respective data
	int c;
  	choice: printf("\nEnter 1 to sort 4 strings as per dictionary order?\n" );
  	printf("Enter 2 to find the index of particular number.\n" );
  	printf("Enter 3 to find factorial of an integer?\n" );
  	printf("Choose the service you want to avail (1/2/3) or -1 to exit: ");
	scanf("%d",&c);
	puts("");

	int temp_factorial;
	double temp_mtrx[5]; //temp_mat = temp_mtrx
	char temp_stg1[100];  //temp_string = temp_stg
	char temp_stg2[100];
	char temp_stg3[100];
	char temp_stg4[100];
	char *temp_arr[4];
	double temp_index;
  
  	if(c==1){
		printf("Enter string1: ");
		        scanf("%s",temp_stg1);
		printf("Enter string2: ");
                scanf("%s",temp_stg2);
		printf("Enter string3: ");
                scanf("%s",temp_stg3);
		printf("Enter string4: ");
                scanf("%s",temp_stg4);
	
		for (int k=0;k<4;k++)
		{
			temp_arr[4];
		}
	}
	else if(c ==2){
		printf("Enter elements of array ( 5 - items): \n");
		for(int i=0;i<5;i++)
				scanf("%lf",&temp_mtrx[i]);
		printf(" enter the number to be searched:\n");
		scanf("%lf",&temp_index);
	}
  	else if(c == 3){
		printf("Enter number to find factorial: ");
		scanf("%d",&temp_factorial);
	}
	else if(c==-1)
		exit(0);
	else{
			printf("Wrong choice..\n");
			goto choice;
	}
	
	//cs start
	if(semop(id, &p, 1) < 0){
    	perror("semop p"); 
     	exit(13);
  	}
		queue->numb = -1;
		queue->numb++;
		int numb = queue->numb;
		queue->que[numb].shared=shmid;
  		queue->que[numb].client=pid;

		if(c == 1){
			queue->que[numb].service = 1;
			strcpy(queue->que[numb].stg1,temp_stg1);
			strcpy(queue->que[numb].stg2,temp_stg2);
			strcpy(queue->que[numb].stg3,temp_stg3);
			strcpy(queue->que[numb].stg4,temp_stg4);
		}
		else if(c == 2){	
			queue->que[numb].service = 2;
			queue->que[numb].inx=temp_index;
			for(int i=0;i<5;i++)
					queue->que[numb].matrix[i] = temp_mtrx[i];
		}
		else if(c == 3){
			queue->que[numb].service = 3;
			queue->que[numb].factorial = temp_factorial;
		}
  
  	if(semop(id, &v, 1) < 0){
    	perror("semop p"); 
    	exit(14);
  	}
	//cs end
  	puts("");

	pause();

	
	double final_ans = myans->ans;
	
	
		if(c==1)
		{			
			printf("%s Sorted\n\n",myans->answer1);
			printf("%s Sorted\n\n",myans->answer2);
			printf("%s Sorted\n\n",myans->answer3);
			printf("%s Sorted\n\n",myans->answer4);
			
			
		}
		else if(c==2)
		{
			
			printf("index is: %0.0lf\n",final_ans);
			if(final_ans == -1)
			{
				printf("that number is not present in the array\n");
			}
		}
		else if (c == 3)
		{
			printf("factorial is : %0.0lf\n",final_ans);
		}
	
  	exit(0);
}
