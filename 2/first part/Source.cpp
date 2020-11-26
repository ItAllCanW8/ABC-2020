#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

using namespace std;

const int NUM_THREADS[] = { 4, 8, 16, 32 };
const int NUM_TASKS = 1048576;

int* arr, index;
bool sleep = false;

mutex mtx;
atomic<int> atomicIndex;

//------------------------------------------------------------
void initArr()
{
	arr = new int[NUM_TASKS];

	for (int i = 0; i < NUM_TASKS; i++)
		arr[i] = 0;
}

//------------------------------------------------------------
int nextIndex()
{
	unique_lock<std::mutex> lock(mtx);

	return index++;
}

//------------------------------------------------------------
void mutexThreads()
{
	while (index < NUM_TASKS)
	{
		arr[nextIndex()] += 1;

		if (sleep)
			this_thread::sleep_for(chrono::nanoseconds(10));
	}
}

//------------------------------------------------------------
void atomicThreads()
{
	while (atomicIndex < NUM_TASKS)
	{
		arr[atomicIndex++] += 1;

		if (sleep)
			this_thread::sleep_for(chrono::nanoseconds(10));
	}
}

//------------------------------------------------------------
int main()
{
	initArr();

	char ch;
	void (*funcPtr)();
	int threadindex;
	bool crrctArrFllng = true;

	cout << "-+=+=+=+=+=+=+=+-+=+=+=+=+=+-+-+-+-+" << endl;
	cout << "func:                               " << endl;
	cout << "1. std::mutex 2. std::atomic        " << endl;

	ch = getchar();

	if (ch == '1')
	{
		funcPtr = mutexThreads;
		index = 0;
	}
	else if (ch == '2')
	{
		funcPtr = atomicThreads;
		atomicIndex = 0;
	}
	else
	{
		cout << "wrong symbol" << endl;
		return 1;
	}

	cout << "num of threads:                     " << endl;
	cout << "1. 4, 2. 8, 3. 16, 4. 32            " << endl;

	cin >> ch;

	if (ch == '1')
		threadindex = 0;
	else if (ch == '2')
		threadindex = 1;
	else if (ch == '3')
		threadindex = 2;
	else if (ch == '4')
		threadindex = 3;
	else
	{
		cout << "wrong symbol" << endl;
		return 1;
	}

	cout << "sleep for 10ns after increment? y/n " << endl;
	cin >> ch;

	if (ch == 'y')
		sleep = true;
	else if (ch == 'n')
		sleep = false;
	else
	{
		cout << "wrong symbol" << endl;
		return 1;
	}

	thread* threads = new thread[NUM_THREADS[threadindex]];

	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < NUM_THREADS[threadindex]; i++)
	{
		threads[i] = thread(funcPtr);
	}

	for (int i = 0; i < NUM_THREADS[threadindex]; i++)
	{
		threads[i].join();
	}

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	for (int i = 0; i < NUM_TASKS; i++)
	{
		if (arr[i] != 1)
		{
			cout << "Error filling array." << endl;
			crrctArrFllng = false;
			break;
		}
	}

	if (crrctArrFllng)
		cout << "The array is filled correctly." << endl;

	cout << "Elapsed time: " << duration.count() << " ms" << endl;

	return 0;
}