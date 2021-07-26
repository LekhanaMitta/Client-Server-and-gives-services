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

struct answers
{
  int ans, error;
  char answer1[100];
  char answer2[100];
  char answer3[100];
  char answer4[100];
};

// Defining comparator function as per the requirement
static int myCompare(const void* a, const void* b)
{

    // setting up rules for comparison
    return strcmp(*(const char**)a, *(const char**)b);
}

// Function to sort the array
void sort(char* arr[], int n)
{
    qsort(arr, n, sizeof(const char*), myCompare);
}

   
int main(int argc, char * argv[]){
  int shared_address = atoi(argv[argc-2]);
  int client_id = atoi(argv[argc-1]);
  struct answers * myans;
  myans = (struct answers *)shmat(shared_address, NULL, 0);
  
  char * arra[4]; 
  signal(SIGUSR1,my_handler);
  

        for(int i=1;i<argc-2;i++)
        {
                arra[i-1]=argv[i];
        }

	int n=argc-3;
    int i;

    // Sort the given names
    sort(arra, n);

	strcpy(myans->answer1,arra[0]);
	strcpy(myans->answer2,arra[1]);
	strcpy(myans->answer3,arra[2]);
	strcpy(myans->answer4,arra[3]);
	
	
	printf("-------Sorted-------\n");
	
    kill(client_id,SIGUSR1);
  return 0;
}
