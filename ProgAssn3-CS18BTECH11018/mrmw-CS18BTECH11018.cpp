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
int n,k,m;
float l1,l2;
default_random_engine generator;
ifstream fileinput;
ofstream fileoutput;

class HelpSnapArray //atomic mrsw register for each thread
{
public:
	int size;
	int *helpsnap1;

	HelpSnapArray() noexcept
	{

	}

	HelpSnapArray(int m)
	{
		size=m;
		helpsnap1=new int[m];
		memset(helpsnap1,INT_MIN,sizeof(helpsnap1));
	}

	HelpSnapArray(int capacity,int *arr)
	{
		size=capacity;
		helpsnap1=arr;
	}

	vector<int> getHelpSnap()
	{
		std::vector<int> v(size);
		for(int i=0;i<size;i++)
		{
			v[i]=helpsnap1[i];
		}
		return v;

	}
};

class StampedSnap
{
public:
	long stamp; //stores stamp
	int value; //stores value of register
	//int sizeofSnapshot;
	int pid; //stores the pid of writer thread
	//int* snap;

	StampedSnap() noexcept //syntax for atomic structs
	{

	}
	StampedSnap(int value)
	{
		stamp=0;
		this->value=value;
	}
	StampedSnap(long label,int value,int pid)
	{
		stamp=label;
		this->value=value;
		this->pid=pid;
	}
	
};

class MRMWSnapshot //mrmw snapshot class
{
public:
	atomic<StampedSnap> *reg; //registers
	atomic<HelpSnapArray> *helpsnap;
	int *sn;
	int snapShotsize;
	MRMWSnapshot(int capacity,int init,int nwriters) //constructor
	{
		snapShotsize=capacity;
		reg = new atomic<StampedSnap>[capacity];
		helpsnap = new atomic<HelpSnapArray>[nwriters];
		sn = new int[nwriters];
		memset(sn,0,sizeof(sn));
		for (int i = 0; i < capacity; ++i) //intialisation
		{
		 	StampedSnap newSnap(init);
		 	reg[i].store(newSnap);
		} 
		for(int i=0;i<nwriters;i++)
		{
			HelpSnapArray newHelp(capacity);
			helpsnap[i].store(newHelp);
		}
	}

	void update(int threadid,int value,int index) //update method for snapshot
	{

		int stampvalue = reg[index].load().stamp;

		sn[threadid]=sn[threadid]+1;

		StampedSnap newValue(sn[threadid],value,threadid);
		reg[index].store(newValue);

		std::vector<int> v=scan();
		int* a=  new int[snapShotsize];
		for(int i=0;i<snapShotsize;i++)
		{
			a[i]=v[i];
		}
		HelpSnapArray updated(snapShotsize,a);
		//for(int i = 0;i<snapShotsize;i++)
		//{
		helpsnap[threadid].store(updated);
		//}
		
	}

	vector<StampedSnap> collect() //get contents of snapshot
	{
		vector<StampedSnap> temp(snapShotsize);
		for (int i = 0; i < snapShotsize; ++i)
		{
			temp[i]=reg[i].load();
		}
		return temp;
	}

	vector<int> scan() //get snapshot
	{
		std::vector<int> canHelp;
		vector<StampedSnap> oldCopy;
		vector<StampedSnap> newCopy;
		oldCopy=collect();
		while(true)
		{
			newCopy=collect();
			bool flag=true;
			for(int i=0;i<snapShotsize;i++)
			{
				if(oldCopy[i].stamp != newCopy[i].stamp || oldCopy[i].value !=newCopy[i].value||oldCopy[i].pid!=newCopy[i].pid)
				{
					flag=false;
					int w = newCopy[i].pid;
					bool twice_modify=false;
					for(int j =0;j<canHelp.size();j++)
					{
						if(j==w)
						{
							twice_modify=true;
							break;
						}
					}
					if(twice_modify)
					{
						return helpsnap[w].load().getHelpSnap();
					}
					else
					{
						canHelp.push_back(w);
					}

				}
			}
			oldCopy=newCopy;
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


void writer(int pid,MRMWSnapshot* mrsw,int m) //driver code for writes
{

	while(!term)
	{
		int vw = rand(); //generating random value
		int l = rand()%m; //random index

		auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string time_var = ctime(&now);

		mrsw->update(pid,vw,l);
		fileoutput<<"Thr"<<pid+1<<"s write of "<<vw<<" on location "<<l+1<<" at "<<time_var.substr(11,8)<<"\n";

		exponential_distribution<double> distribution_one(l1); //sleep 
		auto t1 = distribution_one(generator);
        sleep((int)(t1));
	}
}

void snapshot(MRMWSnapshot* mrsw) //driver code for reader
{

	int i=0;
	exponential_distribution<double> distribution_two(l2);
	while(i<k)
	{
		std::vector<int> v;
		auto beginCollect=chrono::steady_clock::now(); 
		auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string time_var = ctime(&now);
		v=mrsw->scan(); //snapshot
		auto endCollect=chrono::steady_clock::now();
		auto total_time = chrono::duration_cast<chrono::microseconds>(endCollect- beginCollect); //timetaken
		fileoutput<<"Snapshot Thr's snapshot: ";
		for (int i = 0; i < v.size(); ++i)
		{
			fileoutput<<"l"<<i+1<<"-"<<v[i]<<" ";
		}
		fileoutput<<" time ";
		fileoutput<<total_time.count()<<"\n";
		auto t2 = distribution_two(generator);
        sleep((int)(t2));
        i++;
	}
}

int main()
{
	 //to take input
	int l11,l22;
	fileinput.open("inp-params.txt");
	fileinput>>n>>m>>l11>>l22>>k;
	fileinput.close();
	MRMWSnapshot* mrsw = new MRMWSnapshot(m,0,n); //creating snapshot
	term.store(false);
	thread writer_threads[n];
	l1=(1.0)/l11;
	l2=1.0/l22;
	fileoutput.open("output-mrmw.txt");
	for(int i=0;i<n;i++)
	{
        writer_threads[i] = thread(writer,i,mrsw,m);
    }
   // fileoutput<<"create\n";
    thread snapshot_thread(snapshot,mrsw);
    snapshot_thread.join();
    term.store(true);
    for(int i=0;i<n;i++)
    {
        writer_threads[i].join();
    }
    fileoutput.close();
	return 0;
}