#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <queue>

using namespace std;

const int CONSUMER_NUM[] = { 1, 2, 4 };
const int PRODUCER_NUM[] = { 1, 2, 4 };
const int NUM_TASKS = 4194304; //4194304

int consThreadIndx;
int prodThreadIndx;

int counter = 0;
int totalSum = 0;

//------------------------------------------------------------
class Queue
{
private:
	queue<int> queue;
	mutex mtx;

public:

	Queue()
	{

	}

	void push(uint8_t val)
	{
		lock_guard<mutex> guard(mtx);

		queue.push(val);
		counter++;
	}

	bool pop(uint8_t& val)
	{
		lock_guard<mutex> guard(mtx);

		if (!queue.empty())
		{
			val = queue.front();
			queue.pop();

			return true;
		}
		else
			return false;
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
		bool pop = q.pop(val);

		if (!pop && (counter % NUM_TASKS == 0))
		{
			cout << "//Partial sum = " << sum << endl;

			counter = counter - NUM_TASKS;
			totalSum += sum;

			break;
		}
		else
		{
			sum += val;
		}
	}
}

//------------------------------------------------------------
void producerFunc()
{
	int i = 0;

	while (i < NUM_TASKS)
	{
		q.push(1);
		i++;
	}

}

mutex mtx;
condition_variable cv;
queue<uint8_t> fq;

int FIXED_Q_SIZE;
int prodNum;

//------------------------------------------------------------
void producerFunc2()
{
	int i = 0;

	while (i < NUM_TASKS)
	{
		unique_lock<mutex> ul(mtx);
		cv.wait(ul, []() { return fq.size() < FIXED_Q_SIZE; });

		fq.push(1);
		i++;
		counter++;

		ul.unlock();
		cv.notify_all();
	}

}

//------------------------------------------------------------
void consumerFunc2()
{
	int sum = 0;
	uint8_t val = 0;

	while (true)
	{
		unique_lock<mutex> ul(mtx);

		if (fq.size() != 0)
		{
			val = fq.front();
			sum += val;

			fq.pop();
		}

		if (!fq.size() && counter == NUM_TASKS * prodNum)
		{
			cout << "//Partial sum = " << sum << endl;
		
			totalSum += sum;
			counter = counter - NUM_TASKS;
			prodNum--;
		
			break;
		}

		ul.unlock();
		cv.notify_all();
	}
}

//------------------------------------------------------------
int main()
{
	vector<thread> consumers;
	vector<thread> producers;

	cout << "Enter producer thread index (0 -> 1 thread; 1 -> 2; 2 -> 4): ";
	cin >> prodThreadIndx;
	cout << "Enter consumer thread index (0 -> 1 thread; 1 -> 2; 2 -> 4): ";
	cin >> consThreadIndx;
	
	auto start = std::chrono::steady_clock::now();
	
	for (size_t i = 0; i < PRODUCER_NUM[prodThreadIndx]; i++)
		producers.push_back(thread(producerFunc));
	
	for (int i = 0; i < PRODUCER_NUM[prodThreadIndx]; i++)
		producers[i].join();
	
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	
	cout << "\n>Producers have worked for " << duration.count() << " ms" << endl;
	
	start = std::chrono::steady_clock::now();
	
	for (size_t i = 0; i < CONSUMER_NUM[consThreadIndx]; i++)
		consumers.push_back(thread(consumerFunc));
	
	for (int i = 0; i < CONSUMER_NUM[consThreadIndx]; i++)
		consumers[i].join();
	
	end = std::chrono::steady_clock::now();
	duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	
	cout << ">Consumers have worked for " << duration.count() << " ms" << endl;
	
	if (totalSum == PRODUCER_NUM[prodThreadIndx] * NUM_TASKS)
		cout << "(!)Total sum is correct: " << totalSum << endl;

	totalSum = 0;
	counter = 0;

	vector<thread> consumers2;
	vector<thread> producers2;

	cout << "\nEnter Queue Size: ";
	cin >> FIXED_Q_SIZE;
	cout << "Enter producer thread index (0 -> 1 thread; 1 -> 2; 2 -> 4): ";
	cin >> prodThreadIndx;
	cout << "Enter consumer thread index (0 -> 1 thread; 1 -> 2; 2 -> 4): ";
	cin >> consThreadIndx;

	prodNum = PRODUCER_NUM[prodThreadIndx];

	start = std::chrono::steady_clock::now();

	for (size_t i = 0; i < PRODUCER_NUM[prodThreadIndx]; i++)
		producers2.push_back(thread(producerFunc2));

	for (size_t i = 0; i < CONSUMER_NUM[consThreadIndx]; i++)
		consumers2.push_back(thread(consumerFunc2));

	for (int i = 0; i < PRODUCER_NUM[prodThreadIndx]; i++)
		producers2[i].join();

	for (int i = 0; i < CONSUMER_NUM[consThreadIndx]; i++)
		consumers2[i].join();
	
	end = std::chrono::steady_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	cout << "\n>Producers & consumers in total have worked for " << duration.count() << " ms" << endl;

	cout << "Sum = " << totalSum << endl;

	if (totalSum == PRODUCER_NUM[prodThreadIndx] * NUM_TASKS)
		cout << "(!)Total sum is correct: " << totalSum << endl;

	return 0;
}