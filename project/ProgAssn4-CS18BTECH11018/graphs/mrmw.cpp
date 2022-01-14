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




class HelpSnapArray
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
	long stamp;
	int value;
	//int sizeofSnapshot;
	int pid;
	//int* snap;

	StampedSnap() noexcept
	{

	}
	StampedSnap(int value)
	{
		stamp=0;
		this->value=value;
		//this->sizeofSnapshot=sizeofSnapshot;
		//snap=new int[sizeofSnapshot];
		//memset(snap,INT_MIN,sizeof(snap));
	}
	StampedSnap(long label,int value,int pid)
	{
		stamp=label;
		this->value=value;
		//this->sizeofSnapshot=sizeofSnapshot;
		this->pid=pid;
	//	snap=new int[sizeofSnapshot];
	//	memset(snap,INT_MIN,sizeof(snap));
	//	for(int i = 0;i<sizeofSnapshot;i++)
	//	{
	//		snap[i]=snap1[i];
	//	}

	}
	// vector<int> getElements()
	// {
	// 	std::vector<int> v(sizeofSnapshot,0);
	// 	for(int i = 0;i<sizeofSnapshot;i++)
	// 	{
	// 		v[i]=snap[i];
	// 	}
	// 	return v;
	// }
};

class WFSnapshot
{
public:
	atomic<StampedSnap> *reg;
	atomic<HelpSnapArray> *helpsnap;
	int *sn;
	int snapShotsize;
	WFSnapshot(int capacity,int init,int nwriters)
	{
		snapShotsize=capacity;
		reg = new atomic<StampedSnap>[capacity];
		helpsnap = new atomic<HelpSnapArray>[nwriters];
		sn = new int[nwriters];
		memset(sn,0,sizeof(sn));
		for (int i = 0; i < capacity; ++i)
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

	void update(int threadid,int value,int index)
	{
		//fileoutput<<"UU\n";
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
		//int *ptr=a;
		HelpSnapArray updated(snapShotsize,a);
		//for(int i = 0;i<snapShotsize;i++)
		//{
		helpsnap[threadid].store(updated);
		//}
		
	}

	vector<StampedSnap> collect()
	{
		vector<StampedSnap> temp(snapShotsize);
		for (int i = 0; i < snapShotsize; ++i)
		{
			temp[i]=reg[i].load();
		}
		return temp;
	}

	vector<int> scan()
	{
		std::vector<int> canHelp;
		vector<StampedSnap> oldCopy;
		vector<StampedSnap> newCopy;
		//bool* moved = new bool[snapShotsize];
		//memset(moved,false,sizeof(moved));
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
					// if(moved[i])
					// {
					// 	return newCopy[i].getElements();
					// }
					// else
					// {
					// 	moved[i]=true;
					// 	oldCopy=newCopy;
					// 	flag=false;
					// 	break;
					// }

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


void writer(int pid,WFSnapshot* mrsw,int m)
{
	//fileoutput<<"ff\n";
	while(!term)
	{
		int vw = rand();
		int l = rand()%m;
		//fileoutput<<"gg0\n";
		auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string time_var = ctime(&now);
		mrsw->update(pid,vw,l);
	//	fileoutput<<"gg1\n";
		
      //  fileoutput<<"gg2\n";
		//fileoutput<<"Thr"<<pid+1<<"s write of "<<vw<<" on location "<<l+1<<" at "<<time_var.substr(11,8)<<"\n";
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
		auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string time_var = ctime(&now);
		v=mrsw->scan();
		auto endCollect=chrono::steady_clock::now();
		auto total_time = chrono::duration_cast<chrono::microseconds>(endCollect- beginCollect);
	//	fileoutput<<"Snapshot Thr's snapshot: ";
	//	for (int i = 0; i < v.size(); ++i)
	//	{
	//		fileoutput<<"l"<<i+1<<"-"<<v[i]<<" ";
	//	}
	//	fileoutput<<" time ";
	//	fileoutput<<total_time.count()<<"\n";
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
	 //to take input
	n=10,m=20,k=5;
	//fileinput.open("inp-params1.txt");
	//fileinput>>n>>m>>l11>>l22>>k;
	int temp=11;
	float ll1[11]={3,6,12,18,24,30,6,12,15,24,30};
	float ll2[11]={30,30,30,30,30,30,3,3,3,3,3};
	int index=0;
	//fileoutput.op
	//fileinput.open("inp-params.txt");
	//fileinput>>n>>m>>l11>>l22>>k;
	//fileinput.close();
	cout<<"mrmw\n";
	while(index<11)
	{
		WFSnapshot* mrsw = new WFSnapshot(m,0,n);
		term.store(false);
		thread writer_threads[n];
		l2=(1.0)/(ll1[index]);
		l1=1.0/(ll2[index]);
		//fileoutput.open("output.txt");
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
	    cout<<"***********************\n";
	    cout<<"Mue_s "<<ll1[index]<<" Mue_w "<<ll2[index]<<"\n";
	    cout<<"Ratio :"<<double(ll1[index]/ll2[index])<<"\n";
	    cout<<"Avg case: "<<avgcase/k<<"\n";
	    cout<<"worst case: "<<worstcase<<"\n";
	    avgcase=0;
	    worstcase=-1;
	   
	    index++;
	    fileoutput<<"***********************"<<endl;
	    delete mrsw;
	}
	
  //  fileoutput.close();
	return 0;
}