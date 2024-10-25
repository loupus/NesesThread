#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <deque>
#include <future>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <optional>
#include "BackObject.hpp"
#include "Callback.hpp"



namespace NesesUtils
{
    /*
    pooldan task pointer al,
    task callable ve callbackleri set et
    enqueue ederek çalıştır    
    */
    template <typename ReturnType>
    class NesesTask
    {
    private:
        using TaskType = std::packaged_task<ReturnType()>;
        std::atomic<bool> stopflag_;
        TaskType task_;
        std::string name_;

/*         template <typename Func, typename... Args>
        NesesTask(const std::string& name, Func&& func, Args&&... args) : stopflag_(false),name_(name)
        {
            auto boundtask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            task_ = TaskType(boundtask);
        } */

        NesesTask(const std::string& name) : stopflag_(false),name_(name)
        {
        }

    public:
        CallBack<std::string&> cbCompleted_;
        CallBack<int, std::string&> cbProgress_;

        template <typename Func, typename... Args>
        void Set(Func&& func, Args&&... args)
        {
              auto boundtask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            task_ = TaskType(boundtask);
        }

                 
        std::future<ReturnType> GetFuture()
        {  
            if (task_.valid())
                return task_.get_future();
            else
                return std::future<ReturnType>();
        }

        ~NesesTask()
        {

            SetStopFlag(false);
           
        }

        bool GetStopFlag() const
        {
            return stopflag_.load();
        }

        void SetStopFlag(bool stopflag)
        {
            stopflag_.store(stopflag);
        }

        void operator()()
        {
            if (task_.valid())
                task_();

        #if _DEBUG
            else
                std::cout << "invalid task" << std::endl;
        #endif

    
        }

        std::string GetName()
            {
                return name_;
            }

        template<typename T>
        friend class TaskPool;
    };


    template<typename ReturnType>
    class TaskPool
    {
    private:

        size_t maxWorkerCount_;
        const size_t maxTaskCount{ 4 };
        std::vector<std::thread> workers_;
        std::deque<std::shared_ptr<NesesTask<ReturnType>>> tasks;
        std::mutex queueMutex;
        std::condition_variable cv;
        std::atomic<bool> stopFlag;

        void WorkerFunction()
        {
            while (true)
            {
                std::shared_ptr<NesesTask<ReturnType>> task{nullptr};
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    cv.wait(lock, [this] {
                        return stopFlag || !tasks.empty();
                        });

                    if (stopFlag && tasks.empty())
                        return;

                    if (!tasks.empty()) 
                    {
                        task = tasks.front();
                        tasks.pop_front(); 
                    }
                }

                try
                {
                    // Execute the task
                    (*task)();
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Task execution error: " << e.what() << std::endl;
                }
                catch (...)
                {
                    std::cerr << "Unknown error during task execution!" << std::endl;
                }
            }
        }


        void CreateWorker()
        {
            if (workers_.size() < maxWorkerCount_)
            {
                workers_.emplace_back(&TaskPool::WorkerFunction, this);
            }
        }

    public:

        TaskPool()
        {
            maxWorkerCount_ = std::thread::hardware_concurrency();
        }


        ~TaskPool()
        {
            Stop();
        }


        template <typename Func, typename... Args>
        std::shared_ptr<NesesTask<BackObject>> GetNew(Func&& func, Args&&... args)
        {          
            if (stopFlag)
                return nullptr;
            std::shared_ptr<NesesTask<BackObject>> back (new NesesTask<BackObject>(std::forward<Func>(func), std::forward<Args>(args)...));   
            return back;
        }


        bool Enqueue(std::shared_ptr<NesesTask<ReturnType>>& task)
        {
            bool back = false;
            if (tasks.size() >= maxTaskCount)
                return back;
            CreateWorker();
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                tasks.push_back(task);
            }
            cv.notify_one();
            back = true;
            return back;
        }

        void Stop()
        {
            // stop tasks
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                for (auto it = tasks.begin(); it != tasks.end(); it++)
                {
                    (*it)->SetStopFlag(true);
                }
            }

            // stop task pool
            stopFlag = true;
            cv.notify_all();
            for (std::thread& worker : workers_) {
                if (worker.joinable())
                    worker.join();
            }
        }

        size_t workerCount() const
        {
            return tasks.size();
        }

    };

}
