#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <queue>

using namespace std;

const int NUM_TASKS = 4194304;

int prodNum = 1;
int totalSum = 0;

int prodCount = 0;

//------------------------------------------------------------
class Queue
{
private:
	queue<uint8_t> fq;
	mutex mtx;

public:
	void push(uint8_t val)
	{
		lock_guard<mutex> guard(mtx);

		fq.push(val);
	}

	bool pop(uint8_t& val)
	{
		mtx.lock();

		if (fq.empty())
		{
			mtx.unlock();
			this_thread::sleep_for(chrono::milliseconds(1));
			mtx.lock();

			if (fq.empty())
			{
				mtx.unlock();

				return false;
			}
		}

		val = fq.front();
		fq.pop();

		mtx.unlock();

		return true;
	}
};

Queue q;

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
		q.push(1);
}

//------------------------------------------------------------
void output(int consNum, int prodNum)
{
	totalSum = 0;

	vector<thread> consumers;
	vector<thread> producers;

	auto start = std::chrono::steady_clock::now();

	for (size_t i = 0; i < consNum; i++)
		consumers.push_back(thread(consumerFunc));

	auto start2 = std::chrono::steady_clock::now();

	for (size_t i = 0; i < prodNum; i++)
		producers.push_back(thread(producerFunc));

	for (int i = 0; i < consNum; i++)
		consumers[i].join();

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	cout << "\n>Producers (" << prodNum << ") have worked for " << duration.count() << " ms" << endl;

	for (int i = 0; i < prodNum; i++)
		producers[i].join();

	auto end2 = std::chrono::steady_clock::now();
	auto duration2 = std::chrono::duration_cast<std::chrono::seconds>(end2 - start2);

	cout << ">Consumers (" << consNum << ") have worked for " << duration2.count() << " ms" << endl;

	if (totalSum == prodNum * NUM_TASKS)
		cout << "(!)Total sum is correct: " << totalSum << endl;
	else
		cout << "(!)Total sum: " << totalSum << endl;

	cout << "------------------------------------------------------------" << endl;
}

//------------------------------------------------------------
int main()
{
	prodNum = 2;
	output(1, 2);

	prodNum = 4;
	output(1, 4);

	prodNum = 1;
	output(2, 1);

	prodNum = 1;
	output(4, 1);

	prodNum = 4;
	output(2, 4);

	prodNum = 2;
	output(4, 2);

	prodNum = 1;
	output(1, 1);

	prodNum = 2;
	output(2, 2);

	prodNum = 4;
	output(4, 4);

	system("pause");
	return 0;
}