#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<stdbool.h>
#include <sys/msg.h> 
#include<string.h>

void my_handler(){}

struct ANS{
  int answer;
  int err;
};

//factorial
unsigned long long int factorial(unsigned long long int n) { 
  if (n == 0) 
    return 1; 
  return n * factorial(n - 1); 
}

int main(int argc, char * argv[]){
	int shared_address = atoi(argv[argc-2]);
  int client_id = atoi(argv[argc-1]);
  struct ANS *myans = (struct ANS *)shmat(shared_address, NULL, 0);
  signal(SIGUSR1,my_handler);
  
  int num, err;
  unsigned long long int answer;
  
  num = atoi(argv[1]);
  if(num<0) 
    err=1;
  else{	
		err=0;
		answer =factorial(num);
    myans->answer = answer;
	}
	myans->err=err;
	
	printf("-------Factorial Found-------\n");
  kill(client_id,SIGUSR1);
  return 0;
}
