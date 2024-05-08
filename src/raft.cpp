#include "server.h"
#include <mutex>
#include "rpc.h"
#include <memory>
#include <thread>

void raft::server::leader_heart_beat() {
    // 初次唤醒，先发心跳包
    do_heart_beat();
    while(_state == Leader) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::nanoseconds sleep_time;
        {
            std::unique_lock<std::mutex> m_mtx;
            sleep_time = std::chrono::milliseconds(heart_beat_timeout) + last_heart_beat - now;
            sleep_time = std::chrono::milliseconds(sleep_time.count() < 1 ? 1 : sleep_time.count());
        }
        std::this_thread::sleep_for(sleep_time);
        if ((last_heart_beat - now).count() > 0) continue;
        do_heart_beat();
    }
}   // 当被选举为leader后，第一件事就是开一个线程，执行这个任务，等到不是leader后再跳出，避免资源浪费。

void raft::server::do_heart_beat() {
    std::unique_lock<std::mutex> _ul(m_mtx);
    if(_state != Leader) return;
    auto appended_num = std::make_shared<int>(0); // 成功的数量
    for(int i = 0; i < rpcs.size(); i++) {
        if(i == _me) continue;
        if(next_index[i] <= snap_shot_index) {
            // 发送快照 todo
            continue;
        }
        int prev_index = next_index[i] - 1;
        int prev_term = _log->get_entry(prev_index).term;
        auto args = std::make_shared<rpc::append_entry_args>(currentTerm, _me, prev_index, prev_term, commit_index);
        // args->prev_log_index = prev_index;
        for(int j = prev_index + 1; j < last_index + 1; ++j) {
            args->entries.emplace_back(_log->get_entry(j));
        }
        // 多线程 调用append_entries RPC发送包
        // .detach()  todo
    }
    //  重置心跳包时间 todo
}

bool raft::server::append_entries(std::shared_ptr<rpc::append_entry_args>, std::shared_ptr<int> appended_num) {
    
}
