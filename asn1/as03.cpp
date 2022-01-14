#include <bits/stdc++.h>
//#include <pthread.h>
#include <semaphore.h>
#include <thread>
#include <mutex>
#include <fstream>

using namespace std;

long long N;
long long var = 0;
sem_t lock1,lock2,lock3;
mutex incrementLock;
std::vector<long> vdam,vsam;
int n,m;

bool isPrime(long long int x)
{
	if(x==1||x==0)
	{
		return false;
	}
	for(long long int i = 2;i*i<=x;i++)
	{
		if (x%i==0)
		{
			return false;
		}
	}
	return true;
}

void primePrintDAM()
{
	while(var<N)
	{
		sem_wait(&lock1);
		//unique_lock<mutex> wait_lock(incrementLock);
		int temp = var;
		var++;
		//wait_lock.unlock();
		sem_post(&lock1);
		if (isPrime(temp))
		{
			//cout<<temp<<" ";
			sem_wait(&lock2);
			vdam.push_back(temp);
			sem_post(&lock2);
		}

	}

}

void primePrintSAM(int start)
{
	while(start<N)
	{
		//sem_wait(&lock1);
		//unique_lock<mutex> wait_lock(incrementLock);
		//int temp = var;
		//var++;
		//wait_lock.unlock();
		//sem_post(&lock1);
		if (isPrime(start))
		{
			//cout<<temp<<" ";
			sem_wait(&lock3);
			vsam.push_back(start);
			sem_post(&lock3);
		}
		start+=m;

	}

}

int main()
{
	ifstream fileinput;
	fileinput.open("inp-param.txt");
	//int n,m;
	fileinput>>n>>m;
	fileinput.close();
	N = pow(10,n);
	sem_init(&lock1,0,1);
	sem_init(&lock2,0,1);
	sem_init(&lock3,0,1);

	thread primesDam[m];
	auto t1 = chrono::steady_clock::now();
	for (int i = 0; i < m; ++i)
	{
		primesDam[i]=thread(primePrintDAM);
	}
	for (int i = 0; i < m; ++i)
	{
		primesDam[i].join();
	}
	auto t2 = chrono::steady_clock::now();
	auto total_time1 = chrono::duration_cast<chrono::microseconds>(t2 - t1);
	

	thread primesSam[m];
	auto t3 = chrono::steady_clock::now();
	for (int i = 0; i < m; ++i)
	{
		primesSam[i]=thread(primePrintSAM,i+1);
	}
	for (int i = 0; i < m; ++i)
	{
		primesSam[i].join();
	}
	auto t4 = chrono::steady_clock::now();
	auto total_time2 = chrono::duration_cast<chrono::microseconds>(t4 - t3);
	// fileoutput.open("Times.txt");
	// fileoutput<<total_time1.count()<<" ";
	// fileoutput.close();
	ofstream fileoutput;
	fileoutput.open("Primes-DAM.txt");
	for (int i = 0; i < vdam.size(); ++i)
	{
		fileoutput<<vdam[i]<<" ";
	}
	fileoutput.close();
	
	
	//ofstream fileoutput;
	fileoutput.open("Primes-SAM.txt");
	for (int i = 0; i < vsam.size(); ++i)
	{
		fileoutput<<vsam[i]<<" ";
	}
	fileoutput.close();

	fileoutput.open("Times.txt");
	fileoutput<<total_time1.count()<<" "<<total_time2.count()<<"\n";
	fileoutput.close();
	//cout<<"\n";
	return 0;

}