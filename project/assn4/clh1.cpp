#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include <ctime> //for times
#include <chrono>    // for timers 
#include<thread>    //for threads
#include<random> //for exponential distribution

using namespace std;

int n,k;
float l1,l2;
default_random_engine generator;
ifstream fileinput;
//ofstream cout;
double avg_csEnterTime=0,avg_csExitTime=0;
double worst_csEnterTime = -1;

class QNode
{
public:
	bool locked=false;
};

class CLHLock
{
public:
	atomic<QNode*> tail;
	//myPred;
	//myNode;
	CLHLock() noexcept
	{
		atomic_init(&tail,new QNode);
	}

	void lock(QNode *myPred,QNode *myNode)
	{
		QNode *qnode = myNode;
		qnode->locked = true;
		QNode *pred = tail.exchange(qnode);
		myPred = pred;
		while(pred->locked)
		{

		}
	}

	void unlock(QNode *myPred,QNode *myNode)
	{
		QNode *qnode = myNode;
		qnode->locked = false;
		myNode = myPred;
	}
};

void testCS(int pid,CLHLock *clh)
{
	exponential_distribution<double> distribution_one(l1);
    exponential_distribution<double> distribution_two(l2);
    double t1,t2;
  //  QNode a,b;
    QNode *myPred;
    QNode *myNode ;
    for(int i=0;i<k;i++)
    {
    	myNode = new QNode;
    	myPred = NULL;

    	auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    	string print_time = ctime(&now);
    //	cout<<i+1<<" th CS Requested Entry at " <<print_time.substr(11,5)<< " by thread "<<pid+1<<" (mesg1)"<<endl;
    	auto reqEnterTime = chrono::steady_clock::now();
        clh->lock(myPred,myNode);
        auto actEnterTime = chrono::steady_clock::now();
        auto now1 = chrono::system_clock::to_time_t(chrono::system_clock::now());

    	print_time = ctime(&now1);
        auto temp_enter_time = chrono::duration_cast<chrono::seconds>(actEnterTime-reqEnterTime);
        avg_csEnterTime+=temp_enter_time.count();
        if(worst_csEnterTime<temp_enter_time.count())
        {
            worst_csEnterTime=temp_enter_time.count();
        }
        cout<<i+1<<" th CS Entered at " <<print_time.substr(11,5)<< " by thread "<<pid+1<<" "<<endl;

        t1 = distribution_one(generator);
        sleep((int)(t1));

        auto now2 = chrono::system_clock::to_time_t(chrono::system_clock::now());
    	print_time = ctime(&now2);
    	cout<<i+1<<" th CS Requested Exit at " <<print_time.substr(11,5)<< " by thread "<<pid+1<<" "<<endl;
    	auto reqExitTime = chrono::steady_clock::now();
        clh->unlock(myPred,myNode);
        auto actExitTime = chrono::steady_clock::now();

        auto now3 = chrono::system_clock::to_time_t(chrono::system_clock::now());
    	print_time = ctime(&now3);
        auto temp_exit_time = chrono::duration_cast<chrono::seconds>(actEnterTime-reqEnterTime);
        avg_csExitTime+=temp_exit_time.count();
       // cout<<i+1<<" th CS Exit at " <<print_time.substr(11,5)<< " by thread "<<pid+1<<" (mesg4)"<<endl;

        t2 = distribution_one(generator);
        sleep((int)(t2));
    }
}

int main()
{
	float l11,l22;
	fileinput.open("inp-params.txt");
	fileinput>>n>>k>>l11>>l22;
	CLHLock* clh = new CLHLock();
	thread test_threads[n];
	l1=(1.0)/l11; //distribution
	l2=(1.0)/l22;
	//cout.open("output-CLH.txt"); //output file
	for(int i=0;i<n;i++)
	{
        test_threads[i] = thread(testCS,i,clh);
    }

    for(int i=0;i<n;i++)
    {
        test_threads[i].join();
    }
   // cout.close();
	return 0;
}