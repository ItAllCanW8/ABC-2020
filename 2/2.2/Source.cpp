#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <queue>

using namespace std;

const int NUM_TASKS = 4194304;

int totalSum = 0;
int maxFqSize = 1;
int prodNum = 1;
int counter = 0;

bool enough = false;

condition_variable read;
condition_variable write;

//------------------------------------------------------------
class Queue
{
private:
	queue<uint8_t> fq;
	mutex mtx;

public:

	void push(uint8_t val)
	{
		unique_lock<mutex> ul(mtx);
		write.wait(ul, [=]() { return fq.size() < maxFqSize; });
	
		fq.push(val);
		
		read.notify_all();
	}

	bool pop(uint8_t& val)
	{
		unique_lock<mutex> ul(mtx);
		read.wait(ul, [=]() { return fq.size() > 0 || enough;});

		if (!fq.empty())
		{
			val = fq.front();
			fq.pop();
	
			write.notify_all();
			return true;
		}
		else
		{
			return false;
		}
	}
};

Queue q;
mutex mtx;

//------------------------------------------------------------
void consumerFunc()
{
	int sum = 0;
	uint8_t val = 0;

	while (true)
	{
		if (!q.pop(val))
		{
			cout << "//Partial sum = " << sum << endl;
			totalSum += sum;
			break;
		}
		else
			sum += val;
	}
}

//------------------------------------------------------------
void producerFunc()
{
	for (int i = 0; i < NUM_TASKS; i++)
	{ 
		q.push(1);
	}

	mtx.lock();
	counter += NUM_TASKS;
	mtx.unlock();

	if (counter == NUM_TASKS * prodNum)
	{
		enough = true;
		counter = 0;
		read.notify_all();
	}
}

//------------------------------------------------------------
void output(int consNum, int prodNum)
{
	totalSum = 0;
	enough = false;

	vector<thread> consumers;
	vector<thread> producers;

	auto start = std::chrono::steady_clock::now();

	for (size_t i = 0; i < consNum; i++)
		consumers.push_back(thread(consumerFunc));

	auto start2 = std::chrono::steady_clock::now();

	for (size_t i = 0; i < prodNum; i++)
		producers.push_back(thread(producerFunc));

	for (int i = 0; i < prodNum; i++)
		producers[i].join();

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	cout << "\n>QueueSize = " << maxFqSize << ", Producers (" << prodNum << ") have worked for " << duration.count() << " ms" << endl;

	for (int i = 0; i < consNum; i++)
		consumers[i].join();

	auto end2 = std::chrono::steady_clock::now();
	auto duration2 = std::chrono::duration_cast<std::chrono::seconds>(end2 - start2);

	cout << ">QueueSize = " << maxFqSize << ", Consumers (" << consNum << ") have worked for " << duration2.count() << " ms" << endl;

	if (totalSum == prodNum * NUM_TASKS)
		cout << "(!)Total sum is correct: " << totalSum << endl;
	else
		cout << "(!)Total sum: " << totalSum << endl;

	cout << "------------------------------------------------------------" << endl;
}

//------------------------------------------------------------
int main()
{
	prodNum = 1;
	output(4, 1);

	//maxFqSize = 1;
	//output(1, 1);
	//prodNum = 2;
	//output(2, 2);
	//prodNum = 4;
	//output(4, 4);
	//
	//maxFqSize = 4;
	//prodNum = 1;
	//output(1, 1);
	//prodNum = 2;
	//output(2, 2);
	//prodNum = 4;
	//output(4, 4);
	//
	//maxFqSize = 16;
	//prodNum = 1;
	//output(1, 1);
	//prodNum = 2;
	//output(2, 2);
	//prodNum = 4;
	//output(4, 4);

	system("pause");
	return 0;
}