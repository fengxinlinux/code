/*************************************************************************
	> File Name: SyncQueue.h
	> Author:fengxin 
	> Mail:903087053@qq.com 
	> Created Time: 2017年08月05日 星期六 10时05分56秒
 ************************************************************************/

#ifndef _SYNCQUEUE_H


#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <atomic>
#include <functional>

using namespace std;

/********************************同步队列******************************/

template <typename T>
class SyncQueue
{
public:
    SyncQueue(int maxSize): m_maxSize(maxSize), m_needStop(false) { }

    //添加事件
    void Put(const T& x)
    {
        Add(x);
    }

    //添加事件
    void Put(T && x)
    {
        //调用内部接口,进行完美转发
        Add(std::forward<T>(x));
    }

    //从队列中取事件,取所有事件
    void Take(std::list<T> &list)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        //当不满足任何一个则等待,但是若m_needStop为true是因为任务要终止了所以不阻塞
        m_notEmpty.wait(locker, [this]{return (m_needStop || NotEmpty()); });
        if (m_needStop)
        {
            return;
        }

        list = std::move(m_queue);
        m_notFull.notify_one();
    }

    //取一个事件
    void Take(T &t)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty(); });
        if (m_needStop)
        {
            return;
        }

        t = m_queue.front();
        m_queue.pop_front();
        m_notFull.notify_one();
    }

    //终止同步队列
    void Stop()
    {
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            m_needStop = true;
        }

        m_notFull.notify_all();
        m_notEmpty.notify_all();
    }

    //队列为空
    bool Empty()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.empty();
    }

    //队列为满
    bool Full()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size() == m_maxSize;
    }

    //队列大小
    size_t Size()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size();
    }

    //队列大小
    int Count()
    {
        return m_queue.size();
    }

private:
    //队列不为满
    bool NotFull() const
    {
        bool full = (m_queue.size() >= m_maxSize);
        if (full)
        {
            cout << "the queue is full, need wait..." << endl;
        }

        return !full;
    }

    //队列不为空
    bool NotEmpty() const
    {
        bool empty = m_queue.empty();
        if (empty)
        {
            cout << "the queue is empty, need wait..., 异步层的线程ID: " << this_thread::get_id() << endl;
        }

        return !empty;
    }

    //向队列中添加事件,若不为满且终止标志为false则添加事件
    template <typename F>
    void Add(F && x)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notFull.wait(locker, [this]{return m_needStop || NotFull(); });
        if (m_needStop)
        {
            return;
        }

        m_queue.push_back(std::forward<F>(x));
        m_notEmpty.notify_one();
    }

private:
    //缓冲区
    std::list<T> m_queue;
    //互斥量
    std::mutex m_mutex;
    //队列不为空的条件变量
    std::condition_variable m_notEmpty;
    //队列不为满的条件变量
    std::condition_variable m_notFull;
    //任务队列最大长度
    int m_maxSize;
    //终止的标识,当为true时代表同步队列要终止
    bool m_needStop;
};


#define _SYNCQUEUE_H
#endif
