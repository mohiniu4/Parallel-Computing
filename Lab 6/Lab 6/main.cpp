#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include <mpi.h>

// discretize data[n] into 0s and 1s
//
void discretize(float* data, int n) {
    for (int i = 0; i < n; i++)
        data[i] = (pow(sin(data[i]), cos(data[i])) +
            pow(cos(data[i]), sin(data[i]))) / 2.0f;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "%s : invalid number of arguments\n"
            "Usage: %s no_of_elements\n", argv[0], argv[0]);
        return 1;
    }

    int rank, np;

    // initalize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // retrieve number of elements
    int n = atoi(argv[1]);

    // data to hold the array, distrubute the data between the processes and hold the results
    float* data = NULL;
    int chunkSize = n / np; 
    float* result = NULL;

    if (rank == 0) {
        // holds the results of the calculation
        float* result = (float*)malloc(n * sizeof(float)); 

        // initialization
        data = (float*)malloc(n * sizeof(float));
        for (int i = 0; i < n; i++)
            data[i] = (float)rand() / RAND_MAX;

        // scatter the data to all the processes
        MPI_Scatter(data, chunkSize, MPI_FLOAT, MPI_IN_PLACE, chunkSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

        // discretization and collation
        discretize(data, n);

        // collect the data from all the processes
        MPI_Gather(data, chunkSize, MPI_FLOAT, result, chunkSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

        int zeros = 0;
        for (int i = 0; i < n; i++)
            if (result[i] < 0.707f)
                zeros++;

        // report
        printf("%d = %d 0s + %d 1s\n", n, zeros, n - zeros);

    }
    else {
        // allocate memory for the chunk of data that the non-root process will work on
        data = (float*)malloc(chunkSize * sizeof(float));

       // receive the chunk of data from main process
        MPI_Scatter(NULL, chunkSize, MPI_FLOAT, data, chunkSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

        // discretization and collation
        discretize(data, chunkSize);

        // send the chunk of data back to main process
        MPI_Gather(data, chunkSize, MPI_FLOAT, NULL, chunkSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

    }

    // clean up
    free(data);
    free(result);

    MPI_Finalize();
    
    return 0;
}