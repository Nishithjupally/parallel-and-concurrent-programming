#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include<mutex>
#include <ctime> //for times
#include <chrono>    // for timers 
#include<thread>    //for threads
#include<random> //for exponential distribution

using namespace std;

int n,k;
float l1,l2;
default_random_engine generator;
ifstream fileinput;
ofstream fileoutput;


class node
{
public:
	int key;
	mutex node_lock;
	bool marked;
	node* next;
	node()
	{
		key = -1;
		next = NULL;
		marked = false;

	}
	node(int key)
	{
		this->key=key;
		next = NULL;
		marked = false;
	}
};

class Lazy
{

public:
	node* head;
	node* tail;
	Lazy()
	{
		head = new node();
		tail = new node();
		head->key = INT_MIN;
		tail->key = INT_MAX;
		head->next = tail;
		tail->next = NULL;

	}

	bool add(int item)
	{
		while(1)
		{
			node* pred = head;
			node* curr = pred->next;
			while(curr->key < item)
			{
				pred = curr;
				curr = curr->next;
			}
			if(pred->node_lock.try_lock())
			{
				if(curr->node_lock.try_lock())
				{
					if(validate(pred,curr))
					{
						if(curr->key==item)
						{
							pred->node_lock.unlock();
							curr->node_lock.unlock();
							return false;
						}
						else
						{
							node* new_node = new node(item);
							new_node->next = curr;
							pred->next = new_node;
							pred->node_lock.unlock();
							curr->node_lock.unlock();
							return true;

						}
					}
					pred->node_lock.unlock();
					curr->node_lock.unlock();
				}
				else
				{
					pred->node_lock.unlock();
				}
			}
			//curr->node_lock.lock();
			

		}
	}

	bool remove(int item)
	{
		while(1)
		{
			node* pred = head;
			node* curr = (pred->next);
			while(curr->key < item)
			{
				pred = curr;
				curr = curr->next;
			}
			if(pred->node_lock.try_lock())
			{
				if(curr->node_lock.try_lock())
				{
					if(validate(pred,curr))
					{
						if(curr->key==item)
						{
							curr->marked = true;
							pred->next = curr->next;
							pred->node_lock.unlock();
							curr->node_lock.unlock();
							return true;
						}
						else
						{
				
							pred->node_lock.unlock();
							curr->node_lock.unlock();
							return false;

						}
					}
					pred->node_lock.unlock();
					curr->node_lock.unlock();
				}
				else
				{
					pred->node_lock.unlock();
				}
			}
		}
	}
	bool contains(int item)
	{
		node* curr = head;
		while(curr->key<item)
		{
			curr = curr->next;
		}
		return (curr->key==item && !curr->marked);
	}
	private:
	bool validate(node *pred,node *curr)
	{
		return ((!pred->marked) && (!curr->marked) && (pred->next ==curr));
	}
};

//Lazy* list1 = new Lazy();

void test(int pid,Lazy* list1)
{
	//fileoutput<<"f";
	for(int i=0;i<k;i++)
	{
		int x = rand()%3;
		int number = rand()%(k*n);
		bool result;
		//auto newCollect=chrono::steady_clock::now();
		auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string time_var = ctime(&now);
        if(x==0)
        {
        	result = list1->add(number);
        	//auto endCollect=chrono::steady_clock::now();
			//auto total_time = chrono::duration_cast<chrono::microseconds>(endCollect- newCollect);
			//add_t+=otal_time.count();
			//add_ops+=1;
        	// auto now1 = chr

        	 auto now1 = chrono::system_clock::to_time_t(chrono::system_clock::now());
        	 string time_var1 = ctime(&now1);
        	 string s;
        	 if(result==false)
        	 {
        	 	s="false";
        	 }
        	 else
        	 {
        	 	s="true";
        	 }
       		// string time_var1 = ctime(&now1);
      		 fileoutput<<"Thread "<<pid+1<<" performs "<<"add"<<" with number "<<number<<" in round "<<i+1<<" with results "<<s<<" at "<<time_var1.substr(11,8)<<"\n";
        }
        else if(x==1)
        {
        	result = list1->remove(number);
        	//auto endCollect=chrono::steady_clock::now();
			//auto total_time = chrono::duration_cast<chrono::microseconds>(endCollect- newCollect);
			//remove_t+=otal_time.count();
			//remove_ops+=1;
        	 auto now1 = chrono::system_clock::to_time_t(chrono::system_clock::now());
       		 string time_var1 = ctime(&now1);
       		 string s;
        	 if(result==false)
        	 {
        	 	s="false";
        	 }
        	 else
        	 {
        	 	s="true";
        	 }
      		 fileoutput<<"Thread "<<pid+1<<" performs "<<"remove"<<" with number "<<number<<" in round "<<i+1<<" with results "<<s<<" at "<<time_var1.substr(11,8)<<"\n";
        }
        else
        {
        	result = list1->contains(number);
        	//auto endCollect=chrono::steady_clock::now();
			//auto total_time = chrono::duration_cast<chrono::microseconds>(endCollect- newCollect);
			//contains_t+=otal_time.count();
			//contains_ops+=1;
        	 auto now1 = chrono::system_clock::to_time_t(chrono::system_clock::now());
       		 string time_var1 = ctime(&now1);
       		 string s;
        	 if(result==false)
        	 {
        	 	s="false";
        	 }
        	 else
        	 {
        	 	s="true";
        	 }
      		 fileoutput<<"Thread "<<pid+1<<" performs "<<"contains"<<" with number "<<number<<" in round "<<i+1<<" with results "<<s<<" at "<<time_var1.substr(11,8)<<"\n";
        }

        exponential_distribution<double> distribution_one(l1);  
		auto t1 = distribution_one(generator);
        sleep((int)(t1));


	}
}



int main()
{
	int l;
	fileinput.open("inpparams.txt");
	fileinput>>n>>k>>l;
	l1 = 1.0/l;
	fileinput.close();
	//fileoutput<<"k";
	Lazy* list1 = new Lazy();
	thread test_threads[n];
	fileoutput.open("output-lazySynch.txt");
	for(int i =0;i<n;i++)
	{
		test_threads[i] = thread(test,i,list1);
	}

	for(int i=0;i<n;i++)
    {
        test_threads[i].join();
    }
  //  cout<<"Avg time for add"<<add_t/add_ops<<"\n";
    //cout<<"Avg time for remove"<<remove_t/remove_ops<<"\n";
    //cout<<"Avg time for contains"<<contain_t/contain_ops<<"\n";
    fileoutput.close();
	return 0;
}