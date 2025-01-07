#include <stdio.h>
#include <mpi.h>

const unsigned int MASTER = 0;
const unsigned int FROM_MASTER = 1;
const unsigned int FROM_WORKER = 2;


int main(int argc, char* argv[])
{
    int taskid, numtasks, numworkers;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    numworkers = numtasks - 1;

    if (taskid == MASTER)
    {
        int Original_Array[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        int Prefix_Array[10];
        int sum = 0;

        printf("MPI Master starting to work\n");
        printf("Number of workers = %d\n", numworkers);

        //First value of prefix array is the same as the original
        Prefix_Array[0] = Original_Array[0];

        /******   YOU CODE GOES HERE   ******/
        // distribute the work of the array between the workers (tasks)3
        for (int worker = 1; worker <= numworkers; worker++) {
            MPI_Send(Original_Array, 10, MPI_INT, worker, 0, MPI_COMM_WORLD);
        }

        // receive the prefix array values from the works and make it back into the prefix array
        for (int worker = 1; worker <= numworkers; worker++)
        {
            int rec_sum;
            MPI_Recv(&rec_sum, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            Prefix_Array[status.MPI_SOURCE] = rec_sum;
        }
        
        // print out the prefix array
        printf("\nPrefix Sum Array\n");
        for (int count = 0; count < 10; count++)
            printf("%d ", Prefix_Array[count]);
    }

    if (taskid > MASTER)            //A worker task
    {
        int Original_Array[10];
        int sum = 0;
        /******   YOU CODE GOES HERE   ******/
        // recieve the orginal array values from master
        MPI_Recv(Original_Array, 10, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);

        // use the orginal array to calculate the prefix array values and store it in sum
        for (int i = 0; i <= taskid; i++) {
            sum += Original_Array[i];
        }

        // sends sum  to master
        MPI_Send(&sum, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 1;
}