#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace std;

void Thread1Logic() {
	cout << "Thread 1 is running" << endl;
	while (true) {
		cout << this_thread::get_id() << ": Cleanup Complete" << endl;
		this_thread::sleep_for(std::chrono::seconds(1));
	}
	// this will never be reached because of the infinite loop
	cout << "Thread 1 is complete" << endl;
}

void Thread3Logic(string name) {
	cout << "Thread 3 " << name << " is running" << endl;
	this_thread::sleep_for(std::chrono::seconds(5)); // block thread for 5 seconds
	cout << "Thread 3 complete" << endl;
}

void Thread4Logic(string name) {
	cout << "Thread 4 " << name << " is running" << endl;
	this_thread::sleep_for(std::chrono::seconds(2)); // block thread for 2 seconds 
	cout << "Thread 4 complete" << endl;
}

void Thread2Logic() {
	cout << "Thread 2 is running" << endl;

	// create thread 3 & 4 and tranition T2 to a blocked
	// state until they both have been completed
	std::thread T3(Thread3Logic, "nate");
	std::thread T4(Thread4Logic, "john");
	
	// wait for thread 3 & 4 to finish
	T3.join(); // block until thread 3 finishes
	T4.join(); // block until thread 4 finishes

	cout << "Thread 2 is complete because T3 and T4 have been completed" << endl;
}


int main(void) {
	// creating T1 and making it a deamon thread
	std::thread T1(Thread1Logic);
	T1.detach();

	// creating T2
	std::thread T2(Thread2Logic);

	// see if thread 1 & 2 are joinable 
	cout << "Is Thread 1 Joinable? " << (T1.joinable() ? "YES" : "NO") << endl;
	cout << "Is Thread 2 Joinable? " << (T2.joinable() ? "YES" : "NO") << endl;

	// block and wait for T2 to join
	T2.join();

	cout << "Main Thread is now terminating" << endl;
}