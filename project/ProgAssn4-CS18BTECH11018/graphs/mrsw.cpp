#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include <ctime> //for times
#include <chrono>    // for timers 
#include<thread>    //for threads
#include<random> //for exponential distribution

using namespace std;


int x=1000;
atomic<bool> term;
//bool term = false;
int n,k,m;
float l1,l2;
default_random_engine generator;
ifstream fileinput;
ofstream fileoutput;
double avgcase=0;
double worstcase=-1;

class StampedSnap
{
public:
	long stamp;
	int value;
	int sizeofSnapshot;
	int* snap;

	StampedSnap() noexcept
	{

	}
	StampedSnap(int value,int sizeofSnapshot)
	{
		stamp=0;
		this->value=value;
		this->sizeofSnapshot=sizeofSnapshot;
		snap=new int[sizeofSnapshot];
		memset(snap,INT_MIN,sizeof(snap));
	}
	StampedSnap(long label,int value,int sizeofSnapshot,vector<int> snap1)
	{
		stamp=label;
		this->value=value;
		this->sizeofSnapshot=sizeofSnapshot;
		snap=new int[sizeofSnapshot];
		memset(snap,INT_MIN,sizeof(snap));
		for(int i = 0;i<sizeofSnapshot;i++)
		{
			snap[i]=snap1[i];
		}

	}
	vector<int> getElements()
	{
		std::vector<int> v(sizeofSnapshot,0);
		for(int i = 0;i<sizeofSnapshot;i++)
		{
			v[i]=snap[i];
		}
		return v;
	}
};

class WFSnapshot
{
public:
	atomic<StampedSnap> *a_table;
	int snapShotsize;
	WFSnapshot(int capacity,int init)
	{
		snapShotsize=capacity;
		a_table = new atomic<StampedSnap>[capacity];
		for (int i = 0; i < capacity; ++i)
		 {
		 	StampedSnap newSnap(init,capacity);
		 	a_table[i].store(newSnap);
		 } 
	}

	void update(int threadid,int value)
	{
		//fileoutput<<"UU\n";
		int stampvalue = a_table[threadid].load().stamp;
		std::vector<int> v=scan();

		StampedSnap newValue(stampvalue+1,value,snapShotsize,v);
		// std::vector<int> v1=newValue.getElements();
		// for(auto x:v1)
		// {
		// 	fileoutput<<x<<" ";
		// }
		a_table[threadid].store(newValue);
	}

	vector<StampedSnap> collect()
	{
		vector<StampedSnap> temp(snapShotsize);
		for (int i = 0; i < snapShotsize; ++i)
		{
			temp[i]=a_table[i].load();
		}
		return temp;
	}

	vector<int> scan()
	{
		vector<StampedSnap> oldCopy;
		vector<StampedSnap> newCopy;
		bool* moved = new bool[snapShotsize];
		memset(moved,false,sizeof(moved));
		oldCopy=collect();
		while(true)
		{
			newCopy=collect();
			bool flag=true;
			for(int i=0;i<snapShotsize;i++)
			{
				if(oldCopy[i].stamp != newCopy[i].stamp)
				{
					if(moved[i])
					{
						return newCopy[i].getElements();
					}
					else
					{
						moved[i]=true;
						oldCopy=newCopy;
						flag=false;
						break;
					}
				}
			}
			if(flag==false)
			{
				continue;
			}
			std::vector<int> v;
			for (int i = 0; i < snapShotsize; ++i)
			{
				v.push_back(newCopy[i].value);
			}
			return v;
			
		}

	}

};


void writer(int pid,WFSnapshot* mrsw)
{
	//fileoutput<<"ff\n";
	while(!term)
	{
		int vw = rand();
		//fileoutput<<"gg0\n";
		mrsw->update(pid,vw);
	//	fileoutput<<"gg1\n";
		auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string time_var = ctime(&now);
      //  fileoutput<<"gg2\n";
		//fileoutput<<"Thr"<<pid+1<<"s write of "<<vw<<" on location "<<pid+1<<" at "<<time_var.substr(11,8)<<"\n";
		exponential_distribution<double> distribution_one(l1);
		auto t1 = distribution_one(generator);
        sleep((int)(t1));
	}
}

void snapshot(WFSnapshot* mrsw)
{
	//fileoutput<<"ff2\n";
	int i=0;
	exponential_distribution<double> distribution_two(l2);
	while(i<k)
	{
		std::vector<int> v;
		auto beginCollect=chrono::steady_clock::now(); 
		v=mrsw->scan();
		auto endCollect=chrono::steady_clock::now();
		auto total_time = chrono::duration_cast<chrono::microseconds>(endCollect- beginCollect);
		// fileoutput<<"\n";
		// fileoutput<<"Snapshot Thr's snapshot: ";
		// for (int i = 0; i < v.size(); ++i)
		// {
		// 	fileoutput<<"l"<<i+1<<"-"<<v[i]<<" ";
		// }
		// fileoutput<<" time ";
		// fileoutput<<total_time.count()<<"\n";
		// //typeof(total_time.count());
		// fileoutput<<"\n";
		auto t2 = distribution_two(generator);
        sleep((int)(t2));
        avgcase=avgcase+total_time.count();
        if(total_time.count()>worstcase)
        {
        	worstcase=total_time.count();
        }
        i++;
	}
}

int main()
{
	//float l11,l22;
	n=10,m=10,k=5;
	//fileinput.open("inp-params1.txt");
	//fileinput>>n>>m>>l11>>l22>>k;
	int temp=11;
	float ll1[11]={3,6,12,18,24,30,6,12,16,24,30};
	float ll2[11]={30,30,30,30,30,30,3,3,3,3,3};
	int index=0;
	//fileoutput.open("times-mrsw.txt");
	while(index<11)
	{
		WFSnapshot* mrsw = new WFSnapshot(m,0);
		term.store(false);
		thread writer_threads[n];
		l2=(1.0)/(ll1[index]);
		l1=1.0/(ll2[index]);
		
		for(int i=0;i<n;i++)
		{
	        writer_threads[i] = thread(writer,i,mrsw);
	    }
	   // fileoutput<<"create\n";
	    thread snapshot_thread(snapshot,mrsw);
	    snapshot_thread.join();
	    term.store(true);
	    for(int i=0;i<n;i++)
	    {
	        writer_threads[i].join();
	    }
	    cout<<"************m***********\n";
	    cout<<"Mue_s "<<ll1[index]<<" Mue_w "<<ll2[index]<<"\n";
	    cout<<"Ratio :"<<double(ll1[index]/ll2[index])<<"\n";
	    cout<<"Avg case: "<<avgcase/k<<"\n";
	    cout<<"worst case: "<<worstcase<<"\n";
	    avgcase=0;
	    worstcase=-1;
	   
	    index++;
	    fileoutput<<"***********m************"<<endl;
	    delete mrsw;
	}
	// fileoutput.close();
	return 0;
}