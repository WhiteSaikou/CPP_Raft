#pragma once
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <queue>
#include <future>
#include <vector>
#include <type_traits>
#include <functional>
#include <iostream>
#define ASSERT(message, condition) \
    {if (!(condition)) { \
        std::cerr << "Assertion failed: " << message << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::abort(); \
    }}


namespace raft {


    class thread_pool {
      private:
        std::mutex _task_mutex;
        std::queue<std::function<void()>> _task_queue;
        sem_t _task_semaphore;
        bool stop;
        std::vector<std::thread> _task_threads;

      public:
        static thread_pool& get(int num) {
            static thread_pool thp(num);
            return thp;
        }

        template <typename F, typename... Args>
        auto push(F &&func, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
            using T = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared<std::packaged_task<T()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));

            std::future<T> res = task->get_future();
            {
                std::unique_lock<std::mutex> _ul(_task_mutex);
                ASSERT("push a task in a stopped thread pool!", !stop);
                _task_queue.emplace([task]{(*task)();});
            }
            sem_post(&_task_semaphore);
            
            return res;
        }

      private:
        void worker() {
            while(1) {
                std::function<void()> task;
                {
                    sem_wait(&_task_semaphore);
                    std::unique_lock<std::mutex> ul(_task_mutex);
                    if(stop && _task_queue.empty()) break;;
                    task = _task_queue.front();
                    _task_queue.pop();
                }
                task();
            }
        }



      private:
        thread_pool() = delete;
        thread_pool(const thread_pool&) = delete;
        thread_pool(thread_pool&&) = delete;
        const thread_pool& operator=(const thread_pool&) = delete;
        const thread_pool& operator=(thread_pool&&) = delete;

        thread_pool(int thread_num) {
            _task_threads.reserve(thread_num);
            sem_init(&_task_semaphore, 0, 0);
            while(thread_num--) {
                _task_threads.push_back(std::thread(&thread_pool::worker,this));
            }
            stop = false;
            
        }

        ~thread_pool() {
            {
                std::unique_lock<std::mutex> ul(_task_mutex);
                stop = true;
            }
            int num = _task_threads.size();
            while(num--) {
                sem_post(&_task_semaphore);
            }
            for(auto &t : _task_threads) {
                t.join();
            }
            sem_destroy(&_task_semaphore);
        }


    };


};