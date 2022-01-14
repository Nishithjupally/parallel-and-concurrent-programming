#include <bits/stdc++.h>
//#include <pthread.h>
#include <semaphore.h>
#include <thread>
#include <mutex>
#include <fstream>

using namespace std;

long long N;
long long var = 0;
sem_t lock2;
mutex incrementLock;
std::vector<long> v;

bool isPrime(int x)
{
	if(x==1||x==0)
	{
		return false;
	}
	for(int i = 2;i<=sqrt(x);i++)
	{
		if (x%i==0)
		{
			return false;
		}
	}
	return true;
}

void primePrint()
{
	while(var<N)
	{
		//sem_wait(&lock1);
		unique_lock<mutex> wait_lock(incrementLock);
		int temp = var;
		var++;
		wait_lock.unlock();
		//sem_post(&lock1);
		if (isPrime(temp))
		{
			//cout<<temp<<" ";
			sem_wait(&lock2);
			v.push_back(temp);
			sem_post(&lock2);
		}

	}

}

int main()
{
	ifstream fileinput;
	fileinput.open("inp-param.txt");
	int n,m;
	fileinput>>n>>m;
	fileinput.close();
	N = pow(10,n);
	sem_init(&lock2,0,1);
	thread primes[m];
	auto t1 = chrono::steady_clock::now();
	for (int i = 0; i < m; ++i)
	{
		primes[i]=thread(primePrint);
	}
	for (int i = 0; i < m; ++i)
	{
		primes[i].join();
	}
	auto t2 = chrono::steady_clock::now();
	auto total_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
	cout<<total_time.count()<<"\n";
	ofstream fileoutput;
	fileoutput.open("Primes-DAM.txt");
	for (int i = 0; i < v.size(); ++i)
	{
		fileoutput<<v[i]<<" ";
	}
	//cout<<"\n";
	return 0;

}