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

class QNode
{
public:
    bool locked=false;
    QNode* next = NULL;
};

class MCSLock
{
public:
    atomic<QNode*> tail;
    //myNode;
    MCSLock() noexcept
    {
        atomic_init(&tail,NULL);
    }

    void lock(QNode *myNode)
    {
        QNode *qnode = myNode;
        QNode *pred = tail.exchange(qnode);
        if(pred != NULL)
        {
            qnode->locked=true;
            pred->next=qnode;
            while(qnode->locked)
            {

            }
        }
    }

    void unlock(QNode *myNode)
    {
        QNode *qnode = myNode;
        if(qnode->next==NULL)
        {
            if(tail.compare_exchange_strong(qnode,NULL))
            {
                return;
            }
            while(qnode->next==NULL)
            {

            }
        }
        qnode->next->locked = false;
        qnode->next = NULL;
    }
};

void testCS(int pid,MCSLock *MCS)
{
    exponential_distribution<double> distribution_one(l1);
    exponential_distribution<double> distribution_two(l2);
    double t1,t2;
  //  QNode a,b;
    //QNode *myPred;
    QNode *myNode ;
    for(int i=0;i<k;i++)
    {
        myNode = new QNode;
    //  myPred = NULL;

        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string print_time = ctime(&now);
    //  cout<<i+1<<" th CS Requested Entry at " <<print_time.substr(11,5)<< " by thread "<<pid+1<<" (mesg1)"<<endl;
        auto reqEnterTime = chrono::steady_clock::now();
        MCS->lock(myNode);
        auto actEnterTime = chrono::steady_clock::now();
        auto now1 = chrono::system_clock::to_time_t(chrono::system_clock::now());

        print_time = ctime(&now1);
        auto temp_enter_time = chrono::duration_cast<chrono::seconds>(actEnterTime-reqEnterTime);
        avg_csEnterTime+=temp_enter_time.count();
       // cout<<i+1<<" th CS Entered at " <<print_time.substr(11,5)<< " by thread "<<pid+1<<" "<<endl;

        t1 = distribution_one(generator);
        sleep((int)(t1));

        auto now2 = chrono::system_clock::to_time_t(chrono::system_clock::now());
        print_time = ctime(&now2);
       // cout<<i+1<<" th CS Requested Exit at " <<print_time.substr(11,5)<< " by thread "<<pid+1<<" "<<endl;
        auto reqExitTime = chrono::steady_clock::now();
        MCS->unlock(myNode);
        auto actExitTime = chrono::steady_clock::now();

        auto now3 = chrono::system_clock::to_time_t(chrono::system_clock::now());
        print_time = ctime(&now3);
        auto temp_exit_time = chrono::duration_cast<chrono::seconds>(actExitTime-reqExitTime);
        avg_csEnterTime+=temp_exit_time.count();
       // cout<<i+1<<" th CS Exit at " <<print_time.substr(11,5)<< " by thread "<<pid+1<<" (mesg4)"<<endl;

        t2 = distribution_one(generator);
        sleep((int)(t2));
    }
}

int main()
{
    float l11,l22;
    //fileinput.open("inp-params.txt");
    //fileinput>>n>>k>>l11>>l22;
    k = 10;
    l11 = 1;
    l22 = 2; 
    
    l1=(1.0)/l11; //distribution
    l2=(1.0)/l22;
    //cout.open("output-MCS.txt"); //output file
    n = 2;
    cout<<"MCS:\n";
    while(n<128)
    {
        thread test_threads[n];
        MCSLock* MCS = new MCSLock();
      //  cout<<"tt\n";
        for(int i=0;i<n;i++)
        {
            test_threads[i] = thread(testCS,i,MCS);
        }
        //cout<<"f\n";
        for(int i=0;i<n;i++)
        {
            test_threads[i].join();
        }  
        avg_csEnterTime=avg_csEnterTime/(k*n);
        avg_csExitTime=avg_csExitTime/(k*n); 
        cout<<"--------------------------\n";
        cout<<"For n = "<<n<<"\n";
        cout<<"avg_csEnterTime = "<<avg_csEnterTime<<"\n";
        cout<<"avg_csExitTime = "<<avg_csExitTime<<"\n";
        cout<<"--------------------------"<<endl;
        n=2*n;
        avg_csExitTime=0;
        avg_csEnterTime=0;

    }
    
   // cout.close();
    return 0;
}