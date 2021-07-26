
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <sys/msg.h> 
#include<sys/ipc.h>
#include<sys/shm.h>
#include<signal.h>
#include<ctype.h>

void my_handler(){}

struct ANS
{
  int answer, err;
};


int main(int argc,char* argv[])
{
	int shared_address=atoi(argv[argc-2]);
	int client_id=atoi(argv[argc-1]);
	int p=atoi(argv[argc-3]);
	struct ANS *myans = (struct ANS*)shmat(shared_address,NULL,0);
	signal(SIGUSR1,my_handler);

	int arr[5];
	int answer;
	int err=0,i=1;
	int k;
	for(int j=0;j<5;j++)
	{
		arr[j]=atoi(argv[i]);
		i++;
	}
		
	for( k=0;k<5;k++)
	{
		if(arr[k]==p)
		{
			answer = k;
			break;
		}
		else answer = -1;
	}
	
	myans->answer=answer;
	myans->err=0;
	
	printf("-------Index Found-------\n");
	kill(client_id,SIGUSR1);
	return 0;
}


