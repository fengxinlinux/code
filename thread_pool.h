/*************************************************************************
	> File Name: thread_pool.h
	> Author:fengxin 
	> Mail:903087053@qq.com 
	> Created Time: 2017年08月05日 星期六 10时17分16秒
 ************************************************************************/

#ifndef _THREAD_POOL_H

#include<iostream>
#include<list>
#include<thread>
#include<functional>
#include<memory>
#include<atomic>
#include"SyncQueue.h"

using namespace std;

//传递给同步队列的最大个数
const int MaxTaskCount = 100;
class ThreadPool
{
public:
    using Task = std::function<void()>;
    //构造函数,默认参数hardware_concurrency()获取CPU核心数量
    ThreadPool(int numThreads = std::thread::hardware_concurrency()):m_queue(MaxTaskCount)
    {
        cout << "numThreads: " << numThreads<< endl;
        Start(numThreads);
    }

    ~ThreadPool()
    {
        Stop();
    }

    //保证多线程环境下只调用一次StopThreadGroup函数
    void Stop()
    {
        std::call_once(m_flag, [this]{ StopThreadGroup(); });
    }

    //添加任务,右值完美转发
    void AddTask(Task && task)
    {
        m_queue.Put(std::forward<Task> (task));
    }

    //添加任务
    void AddTask(const Task && task)
    {
        m_queue.Put(task);
    }

private:
    //建立numThreads个数的线程组
    void Start(int numThreads)
    {
        m_running  = true;

        for (int i = 0; i < numThreads; i++)
        {
            //多个线程依次的处理
            m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
        }
    }

    //取出任务队列中的全部,依次执行
    void RunInThread()
    {
        while (m_running)
        {
            std::list<Task> list;
            m_queue.Take(list);

            for (auto & task : list)
            {
                if (!m_running)
                {
                    return ;
                }

                //执行任务
                task();
            }
        }
    }

    //终止所有任务的执行
    void StopThreadGroup()
    {
        //终止同步队列
        m_queue.Stop();
        m_running = false;

        for (auto thread : m_threadgroup)
        {
            if (thread)
            {
                thread->join();
            }
        }

        m_threadgroup.clear();
    }

private:
    //处理任务的线程组
    std::list<std::shared_ptr<std::thread>> m_threadgroup;
    //同步队列
    SyncQueue<Task> m_queue;
    //运行的标志,flase代表终止
    atomic_bool m_running;
    //保证在函数在多线程环境中只被调用一次
    std::once_flag m_flag;
};


#define _THREAD_POOL_H
#endif
