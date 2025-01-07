#include <iostream>
#include <cmath>
#include <omp.h> // header for openmp
using namespace std;

const int Num_Elements = 10;

int main()
{
    int Original_Array[Num_Elements] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int Prefix_Array[Num_Elements];
    int sum = 0;
    int i = 0;
    int step;
    int temp[Num_Elements]; // temporary array to hold the prefix array computations

    cout << "Sequential" << endl;
    cout << "Prefix_Array :- " << " ";

    while (i < 10)
    {
        sum += Original_Array[i];
        Prefix_Array[i] = sum;
        cout << Prefix_Array[i++] << " ";
    }
    cout << endl;

    cout << "Parallel" << endl;
    omp_set_num_threads(2); // set the number of threads to 2
  
    // reinitialize prefix array and sum
    sum = 0;
    for (int i = 0; i < Num_Elements; i++) {
        Prefix_Array[i] = Original_Array[i];
    }

    
    for (int r = 1; r < Num_Elements; r++) {
        step = pow(2, r - 1);

        // computes the prefix sum for the current step and stores the value in the temp array
        #pragma omp parallel for 
        for (int i = step; i < Num_Elements; i++) {
            temp[i] = Prefix_Array[i] + Prefix_Array[i - step];
        }

        // copies the temp values into to prefix array
        for (int i = step; i < Num_Elements; i++) {
            Prefix_Array[i] = temp[i];
        }

        // ensure all threads are done before going to the next section
        #pragma omp barrier
     }

    // prints the values from the parallel prefix sum
    cout << "Prefix_Array :- " << " ";
    for (int i = 0; i < Num_Elements; i++) {
        cout << Prefix_Array[i] << " ";
    }
    cout << endl;

    return 1;
}