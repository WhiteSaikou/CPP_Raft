#include "common/thread_pool.h"
#include <iostream>


#include <chrono>
#include <thread>
#include <unistd.h>

void print(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[PID = " << (gettid())<< " ]" << " x = " << x << std::endl;
    // int i = 10000000;
    // while(i--);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // while(1);
}


int main() {
    raft::thread_pool& thp = raft::thread_pool::get(10);
    int num = 100;
    while(num--) {
        thp.push(print,num);
    }
    return 0;
}