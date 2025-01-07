#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
using namespace std;


const unsigned int NUM_ROWS_A = 1000;
const unsigned int NUM_COLS_A = 1000;
const unsigned int NUM_ROWS_B = NUM_COLS_A;
const unsigned int NUM_COLS_B = 1000;
unsigned int numOfThreads = 4; // Number of threads used


void InitArray(int**& Matrix, unsigned int Rows, unsigned int Cols) {
    Matrix = new int* [Rows];
    if (Matrix == NULL) {
        exit(2);
    }
    for (unsigned int i = 0; i < Rows; i++) {
        Matrix[i] = new int[Cols];
        if (Matrix[i] == NULL) {
            exit(2);
        }
        for (unsigned int j = 0; j < Cols; j++) {
            Matrix[i][j] = rand() % 100 + 1;
        }
    }
}

void DisplayArray(int**& Matrix, unsigned int Rows, unsigned int Cols) {
    for (unsigned int i = 0; i < Rows; i++) {
        for (unsigned int j = 0; j < Cols; j++) {
            cout << " [" << Matrix[i][j] << "] ";
        }
        cout << endl;
    }
}

//Sequential Matrix Multiplication
void sequential_matrix_multiply(int**& Matrix_A, unsigned int num_rows_a, unsigned int num_cols_a,
    int**& Matrix_B, unsigned int num_rows_b, unsigned int num_cols_b,
    int**& Result) {
    for (unsigned int i = 0; i < num_rows_a; i++) {
        for (unsigned int j = 0; j < num_cols_b; j++) {
            Result[i][j] = 0;
            for (unsigned int k = 0; k < num_cols_a; k++) {
                Result[i][j] += Matrix_A[i][k] * Matrix_B[k][j];
            }
        }
    }
}

// Parallel Matrix Multiplication

mutex Mutex;

void parallel_helper(int**& Matrix_A, int**& Matrix_B, int**& Result, unsigned int starting_row, unsigned int ending_row, unsigned int num_cols_a, unsigned int num_cols_b) {
    
    // locking the mutex to stop other threads to access this critial section at the same time 
    Mutex.lock(); 
    for (unsigned int i = starting_row; i < ending_row; i++) {
        for (unsigned int j = 0; j < num_cols_b; j++) {
            Result[i][j] = 0;
            for (unsigned int k = 0; k < num_cols_a; k++) {
                Result[i][j] += Matrix_A[i][k] * Matrix_B[k][j];
            }
        }
    }
    Mutex.unlock(); // unlocking it to make it available to other threads
}

void parallel_matrix_multiply(int**& Matrix_A, unsigned int num_rows_a, unsigned int num_cols_a,
    int**& Matrix_B, unsigned int num_rows_b, unsigned int num_cols_b,
    int**& Result) {

   // vector to hold the threads
   vector <thread> threads(numOfThreads);

   // calculate how much each thread will handle
   unsigned int chunks = num_rows_a / numOfThreads;

   for (unsigned int i = 0; i < numOfThreads; i++) {

       // determining the starting and ending row for the thread
       unsigned int starting_row = i * chunks;
       unsigned int ending_row;
       if (i == numOfThreads - 1) {
           ending_row = num_rows_a;
       }
       else {
           ending_row = (i + 1) * chunks;
       }

       // create the thread
       threads[i] = thread(parallel_helper, ref(Matrix_A), ref(Matrix_B), ref(Result), starting_row, ending_row, num_cols_a, num_cols_b);
   }

   // joining all the threads once they are done
   for (unsigned int j = 0; j < numOfThreads; j++) {
       threads[j].join();
   }
}

int main()
{
    int** MatrixA = nullptr;
    int** MatrixB = nullptr;
    int** Result = nullptr;

    //Allocate data for the resulting arrays
    Result = new int* [NUM_ROWS_A];
    for (unsigned int i = 0; i < NUM_ROWS_A; i++) {
        Result[i] = new int[NUM_COLS_B];
    }

    InitArray(MatrixA, NUM_ROWS_A, NUM_COLS_A);
    InitArray(MatrixB, NUM_ROWS_B, NUM_COLS_B);

    // sequential matrix
    cout << "Evaluating Sequential Task" << endl;
    chrono::duration<double> Seq_Time1(0);
    auto startTime1 = chrono::high_resolution_clock::now();
    sequential_matrix_multiply(MatrixA, NUM_ROWS_A, NUM_COLS_A,
        MatrixB, NUM_ROWS_B, NUM_COLS_B,
        Result);
    Seq_Time1 = chrono::high_resolution_clock::now() - startTime1;

    
    /*DisplayArray(MatrixA, NUM_ROWS_A, NUM_COLS_A);
    cout << endl << endl;
    DisplayArray(MatrixB, NUM_ROWS_B, NUM_COLS_B);
    cout << endl << endl;
    DisplayArray(Result, NUM_ROWS_A, NUM_COLS_B);*/
    

    cout << "FINAL RESULTS" << endl;
    cout << "=============" << endl;
    cout << "Sequential Processing took: " << Seq_Time1.count() * 1000 << endl;

    // Parallel matrix 
    cout << "Evaluating Parallel Task" << endl;
    chrono::duration<double> Seq_Time2(0);
    auto startTime2 = chrono::high_resolution_clock::now();
    parallel_matrix_multiply(MatrixA, NUM_ROWS_A, NUM_COLS_A,
        MatrixB, NUM_ROWS_B, NUM_COLS_B,
        Result);
    Seq_Time2 = chrono::high_resolution_clock::now() - startTime2;

    /*DisplayArray(MatrixA, NUM_ROWS_A, NUM_COLS_A);
    cout << endl << endl;
    DisplayArray(MatrixB, NUM_ROWS_B, NUM_COLS_B);
    cout << endl << endl;
    DisplayArray(Result, NUM_ROWS_A, NUM_COLS_B);*/


    cout << "FINAL RESULTS" << endl;
    cout << "=============" << endl;
    cout << "Parallel Processing took: " << Seq_Time2.count() * 1000 << endl;

    // calculate and output efficiency
    float efficiency = (Seq_Time1.count() / Seq_Time2.count()) / numOfThreads;
    cout << "Efficiency: " << efficiency << endl;

    return 1;
}