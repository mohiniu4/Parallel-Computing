#include <iostream>
#include <thread>
#include <cmath>
#include <mutex>
#include <chrono>
#include <vector>
#include <set> // for storing unqiue thread ids

using namespace std;

mutex merge_mutex; // to protect the merge critical section
vector<thread::id> thread_ids;  // vector to store thread IDs
unsigned int numOfThreads = 16; //16 as thats the number of cores my laptop has

void merge(int* array, unsigned int left, unsigned int mid, unsigned int right);

void sequential_merge_sort(int* array, unsigned int left, unsigned int right) {
	if (left < right) {
		unsigned int mid = (left + right) / 2;
		sequential_merge_sort(array, left, mid);
		sequential_merge_sort(array, mid + 1, right);
		merge(array, left, mid, right);
	}
}

void Parallel_Merge_Sort(int* array, unsigned int left, unsigned int right, unsigned int numOfThreads) {
	if (left < right) {
		// Calculate the midpoint
		unsigned int mid = (left + right) / 2;

		if (numOfThreads > 1) {
			unsigned int half_threads = numOfThreads / 2;

			// create a thread to sort the left side
			thread t1(Parallel_Merge_Sort, array, left, mid, half_threads);

			// sort the right side in the current thread
			Parallel_Merge_Sort(array, mid + 1, right, half_threads);

			// wait for the left side to finish
			t1.join();

			// merge both sides together
			{
				scoped_lock<mutex> guard(merge_mutex); // lock the critical section
				thread_ids.push_back(std::this_thread::get_id()); //store the current threads id
				merge(array, left, mid, right);
			}
		}
		else {
			// only 1 thread avilable so it performs sequential sorting
			sequential_merge_sort(array, left, mid);
			sequential_merge_sort(array, mid + 1, right);

			// merge both sides together
			{
				scoped_lock<mutex> guard(merge_mutex); // lock the critical section
				thread_ids.push_back(std::this_thread::get_id()); //store the current threads id
				merge(array, left, mid, right);
			}

		}
	}
	
}

void merge(int* array, unsigned int left, unsigned int mid, unsigned int right) {
	unsigned int num_left = mid - left + 1; // number of elements in left subarray
	unsigned int num_right = right - mid; // number of elements in right subarray

	// copy data into temporary left and right subarrays to be merged
	int* array_left = new int[num_left];
	int* array_right = new int[num_right];
	copy(&array[left], &array[mid + 1], array_left);
	copy(&array[mid + 1], &array[right + 1], array_right);

	// initialize indices for array_left, array_right, and input subarrays
	unsigned int index_left = 0;    // index to get elements from array_left
	unsigned int index_right = 0;    // index to get elements from array_right
	unsigned int index_insert = left; // index to insert elements into input array

	// merge temporary subarrays into original input array
	while ((index_left < num_left) || (index_right < num_right)) {
		if ((index_left < num_left) && (index_right < num_right)) {
			if (array_left[index_left] <= array_right[index_right]) {
				array[index_insert] = array_left[index_left];
				index_left++;
			}
			else {
				array[index_insert] = array_right[index_right];
				index_right++;
			}
		}
		// copy any remain elements of array_left into array
		else if (index_left < num_left) {
			array[index_insert] = array_left[index_left];
			index_left += 1;
		}
		// copy any remain elements of array_right into array
		else if (index_right < num_right) {
			array[index_insert] = array_right[index_right];
			index_right += 1;
		}
		index_insert++;
	}
}

int main() {
	const int NUM_EVAL_RUNS = 100;
	const int N = 100000;
	
	// Sequential Merge Sort
	cout << "Sequential Merge Sort" << endl;
	chrono::duration<double> Seq_time(0);
	auto startTime1 = chrono::high_resolution_clock::now();
	int original_array[N], result[N + 1];
	for (int i = 0; i < N; i++) {
		original_array[i] = rand();
		//cout << "[" << original_array[i] << "] ";
	}
	//cout << endl;
	copy(&original_array[0], &original_array[N], result);
	sequential_merge_sort(result, 0, N);
	for (int i = 0; i < NUM_EVAL_RUNS; i++) {
		copy(&original_array[0], &original_array[N], result);
		sequential_merge_sort(result, 0, N);
	}
	/*for (int i = 0; i < N; i++) {
		cout << "[" << result[i] << "] ";
	}
	cout << endl;*/
	Seq_time = chrono::high_resolution_clock::now() - startTime1;
	cout << "Sequential Merge Sort took: " << Seq_time.count() << " seconds" << endl;

	// Parallel Merge Sort
	cout << "Parallel Merge Sort" << endl;
	chrono::duration<double> Par_time(0);
	auto startTime2 = chrono::high_resolution_clock::now();
	for (int i = 0; i < N; i++) {
		original_array[i] = rand();
		//cout << "[" << original_array[i] << "] ";
	}
	//cout << endl;
	copy(&original_array[0], &original_array[N], result);
	Parallel_Merge_Sort(result, 0, N - 1, numOfThreads);
	/*for (int i = 0; i < N; i++) {
		cout << "[" << result[i] << "] ";
	}
	cout << endl;*/
	Par_time = chrono::high_resolution_clock::now() - startTime2;
	cout << "Parallel Merge Sort took: " << Par_time.count() << " seconds" << endl;

	// calculate and output efficiency
	float efficiency = (Seq_time.count() / Par_time.count()) / numOfThreads;
	cout << "Efficiency: " << efficiency << endl;

	// Threads in critical section
	cout << "Threads that accessed the critical section:\n";
	set<thread::id> unique_ids(thread_ids.begin(), thread_ids.end());  // Using set to get unique IDs
	for (const auto& id : unique_ids) {
	        cout << id << endl;
    }
}
