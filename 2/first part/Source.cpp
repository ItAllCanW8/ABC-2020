#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <vector>
#include <string>

using namespace std;

const int NUM_TASKS = 1048576;

bool sleep = false;

int index = 0;
atomic<int> atomicIndex{ 0 };

vector<int> arr;
vector<int> arr2;

mutex mtx;

//------------------------------------------------------------
void initArr()
{
	arr.clear();

	for (int i = 0; i < NUM_TASKS; i++)
		arr.push_back(0);		
}

//------------------------------------------------------------
void mutexThreads()
{
	lock_guard<mutex> guard(mtx);

	while (index < NUM_TASKS)
	{
		arr.at(index) += 1;
		index++;

		if (sleep)
			this_thread::sleep_for(chrono::nanoseconds(10));
	}
}

//------------------------------------------------------------
void atomicThreads()
{
	while (atomicIndex.load() < NUM_TASKS)
	{
		int temp = atomicIndex.fetch_add(1);

		if(temp < NUM_TASKS)
			arr2.at(temp)++;

		if (sleep)
			this_thread::sleep_for(chrono::nanoseconds(10));
	}
	
}

//------------------------------------------------------------
void mutexCheck(int numOfThreads, bool sleep)
{
	index = 0;

	initArr();

	vector<thread> threads;

	for (size_t i = 0; i < numOfThreads; i++)
	{
		threads.push_back(thread(mutexThreads));
	}

	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < numOfThreads; i++)
		threads[i].join();

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	bool crrctArrFllng = true;

	for (int i = 0; i < NUM_TASKS; i++)
	{
		if (arr.at(i) != 1)
		{
			cout << "Error filling array." << endl;
			crrctArrFllng = false;
			break;
		}
	}

	if (crrctArrFllng)
		cout << "Mutex, " << numOfThreads << " threads, sleep = " << sleep << ", the array is filled correctly, ";

	cout << "elapsed time: " << duration.count() << " ms" << endl;
}

//------------------------------------------------------------
void atomicCheck(int numOfThreads, bool sleep)
{
	vector<thread> threads;
	atomicIndex = 0;
	arr2.clear();

	for (size_t i = 0; i < NUM_TASKS; i++)
	{
		arr2.push_back(0);
	}

	for (size_t i = 0; i < numOfThreads; i++)
		threads.push_back(thread(atomicThreads));

	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < numOfThreads; i++)
		threads[i].join();

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	bool crrctArrFllng = true;

	for (int i = 0; i < NUM_TASKS; i++)
	{
		if (arr2.at(i) != 1)
		{
			cout << "Error filling array." << endl;
			crrctArrFllng = false;
			break;
		}
	}

	if (crrctArrFllng)
		cout << "Atomic, " << numOfThreads << " threads, sleep = " << sleep << ", the array is filled correctly, ";

	cout << "elapsed time: " << duration.count() << " ms" << endl;
}

//------------------------------------------------------------
int main()
{
	mutexCheck(4, sleep);
	mutexCheck(8, sleep);
	mutexCheck(16, sleep);
	mutexCheck(32, sleep);
	
	sleep = true;
	cout << "---------------------------------------------------------------------------------" << endl;
	
	mutexCheck(4, sleep);
	mutexCheck(8, sleep);
	mutexCheck(16, sleep);
	mutexCheck(32, sleep);

	sleep = false;
	cout << "---------------------------------------------------------------------------------" << endl;

	atomicCheck(4, sleep);
	atomicCheck(8, sleep);
	atomicCheck(16, sleep);
	atomicCheck(32, sleep);
	
	sleep = true;
	cout << "---------------------------------------------------------------------------------" << endl;
	
	atomicCheck(4, sleep);
	atomicCheck(8, sleep);
	atomicCheck(16, sleep);
	atomicCheck(32, sleep);

	return 0;
}