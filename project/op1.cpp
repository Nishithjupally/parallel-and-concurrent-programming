#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include <ctime> //for times
#include <chrono>    // for timers 
#include <thread>    //for threads
#include <random> //for exponential distribution
#include <atomic>

using namespace std;
class pointer_t;
class node_t;
class queue_t;

void fixList(queue_t *q,pointer_t tail,pointer_t head);

class pointer_t{
    public:
    node_t* ptr;
    int tag;
    pointer_t() noexcept
    {
        ptr = NULL;
        tag = 0;
    }
};

class node_t{
    public:
    int value;
    pointer_t next;
    pointer_t prev;
    node_t()
    {
        value = 0;
    }
};

bool comparePointer(pointer_t a,pointer_t b)
{
    if((a.ptr != b.ptr)||(a.tag != b.tag))
    {
        return false;
    }
    return true;
}

class queue_t{
    public:
    atomic<pointer_t> tail;
    atomic<pointer_t> head;
    
    // queue_t() noexcept
    // {

    // }

    queue_t()
    {
        node_t* nd = new node_t();
        
        nd->next.ptr = NULL;
        nd->next.tag = 0;
        pointer_t temp;
        temp.ptr=nd;
        temp.tag=0;
        (tail).store(temp);
       // (q->tail).store().tag = 0;
        (head).store(temp);
       // (q->head).store().tag = 0;
    }

    void enqueue(queue_t* q, int val)
    {
        pointer_t check_tail;
        node_t* nd = new node_t();
        nd->value = val;
        while(1)
        {
            check_tail = (q->tail).load();
            nd->next.ptr = check_tail.ptr;
            nd->next.tag = check_tail.tag+1;
            pointer_t temp;
            temp.ptr = nd;
            temp.tag = check_tail.tag+1;
            if((q->tail).compare_exchange_strong(check_tail, temp)) //
            {
                (check_tail.ptr)->prev.ptr = nd;
                (check_tail.ptr)->prev.tag = check_tail.tag+1;
                break;
            }
        }
    }

    int dequeue(queue_t* q)
    {
        pointer_t check_tail,check_head,firstNodePrev;
        int val;
        while(1)
        {
            check_head = (q->head).load();
            check_tail = (q->tail).load();
            firstNodePrev = (check_head.ptr)->prev;
            if(comparePointer(check_head, (q->head).load()))
            {
                if(!comparePointer(check_tail,check_head))
                {
                    if(firstNodePrev.tag!=check_head.tag)
                    {
                        fixList(q,check_tail,check_head);
                        continue;
                    }
                    val = (firstNodePrev.ptr)->value;
                    pointer_t temp;
                    temp.ptr = firstNodePrev.ptr;
                    temp.tag = check_head.tag+1;
                    if((q->head).compare_exchange_strong(check_head, temp)) //
                    {
                        delete check_head.ptr ;//????
                        return val;
                    }
                }
            }
            else
            {
                return INT_MIN;
            }
        }
    }

};
queue_t opt_q;

void fixList(queue_t *q,pointer_t tail,pointer_t head)
{
    pointer_t curNode, curNodeNext;
    curNode = tail;
    while((comparePointer(head,(q->head.load())))&& (!comparePointer(curNode, head)))
    {
        curNodeNext = (curNode.ptr)->next;
        (curNode.ptr)->prev.ptr = curNode.ptr; 
        (curNode.ptr)->prev.tag = curNode.tag - 1;
        curNode.ptr = curNodeNext.ptr;
        curNode.tag = curNode.tag - 1;
    }
}


void test(int i){
    if(i&1){
        
    }
}

int main(){

    // queue_t q = new 
    thread test_threads[5];
	for(int i=0;i<5;i++)
	{
            test_threads[i] = thread(test_deq,&q);
        
    }

    for(int i=0;i<n;i++)
    {
        test_threads[i].join();
    }
    return 0;
}