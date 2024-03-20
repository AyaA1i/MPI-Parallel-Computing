// Online C compiler to run C program online
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"
// need a function to check if the number is prime or not
// need a counter to increment if yes 
// program inputs: lower and upper ranges , number of processes
//			  (given in running step)

//MASTER TASKS
// calculate r = upper - lower / number of processes
// send: loop on all processors and send them the lower bound and the range
// recieve: the total of the each processor 
// add all totals 
// print the result


//SLAVE TASKS
// recieve the lower bound and the range
// to calculate its own start and end: 
// 		start = processorId-1 * range
// 		end = start + range
// loop and calculate the number of prime numbers in its range
// print the result
// send the result to the master processor


/* utility function to tell whether number is prime or not*/
/* I made it return integer to add the return value to the counter of the slave processors*/
int isPrime(int num)
{
if(num == 1 )
{
    return 0;
}
int i ;
for(i = 2 ; i*i<=num;i++)
	{
		if(num%i == 0) return 0;
	}

return 1; /* it is prime*/
}


int main(int argc , char * argv[])
{
int my_rank; /* rank of process */
int p; /* number of process */
int source; /* rank of sender */
int dest; /* rank of reciever */
int total = 0; // of the whole program
int subtotal = 0; // of each processor
int lower, upper; // of the range
int range;
int start, end; // of the ranges of the processors
MPI_Status status; /* return status for */
/* recieve */

/* Start up MPI */
MPI_Init( &argc , &argv );
/* Find out process rank */
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
/* Find out number of process */
MPI_Comm_size(MPI_COMM_WORLD, &p);

if(my_rank != 0) // i am master
{
     // Slave processes receive lower and range from the master process
        MPI_Recv(&lower, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&range, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
        start = ((my_rank-1)* range)+1;
        end = start+range;
        int i;
        for(i = start ; i<end;i++)
        {
            subtotal += isPrime(i);
        }
        printf( "number of prime numbers for %d is %d !\n",my_rank,subtotal);
        MPI_Send(&subtotal, 1, MPI_INT , 0,0,MPI_COMM_WORLD);

}
else // i am a slave
{
    printf("Enter the lower bound: \n");
    scanf("%d", &lower);

    printf("Enter the upper bound: \n");
    scanf("%d", &upper);
    range = (upper - lower)/(p-1);
    int i ;
    for(i = 1; i<p ; i++)
    {
        MPI_Send( &lower, 1 , MPI_INT, i, 0,MPI_COMM_WORLD);  
        MPI_Send( &range, 1 , MPI_INT, i, 0,MPI_COMM_WORLD);  
    }
    for(i = 1; i<p ; i++)
    {
    MPI_Recv(&subtotal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        total+=subtotal;
    }
    sleep(1);
    printf("total number of prime numbers is %d  !\n",total);
        
}
MPI_Finalize();
}
