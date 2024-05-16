#include "raft.h"
#include <mutex>
#include "rpc.h"
#include <memory>
#include <thread>
#include "thread_pool.h"

void raft::raft_server::leader_heart_beat() {
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

void raft::raft_server::do_heart_beat() {
    std::unique_lock<std::mutex> _ul(m_mtx);
    if(_state != Leader) return;
    auto appended_num = std::make_shared<int>(0); // 成功的数量
    for(int i = 0; i < rpcs.size(); i++) {
        if(i == _me) continue;
        // if(next_index[i] <= snap_shot_index) {
        //     // 发送快照 todo
        //     continue;
        // }

        // 多线程 调用append_entries RPC发送包
        thread_pool::get().push([this, i, appended_num](){
            append_entries(i, appended_num);
        });
    }
    //  重置心跳包时间 todo
}

bool raft::raft_server::append_entries(int server_id,  std::shared_ptr<int> appended_num) {
    int prev_index = next_index[server_id] - 1;
    int prev_term = _log->get_entry(prev_index).term;
    auto args = std::make_shared<rpc::append_entry_args>(current_term, _me, prev_index, prev_term, commit_index);
    for(int j = prev_index + 1; j < last_index + 1; ++j) {
        args->entries.emplace_back(_log->get_entry(j));
    }

    if(_state != Leader) return false;           // 保证发送的一定是leader
    auto reply = rpcs[server_id]->request(args); // 内部使用TCP，所以不用手动实现 5.3节的重复发送

    if(reply == nullptr) return false;
    std::unique_lock<std::mutex> _ul(m_mtx);
    if(reply->term > current_term) {
        current_term = reply->term;
        _state = Follower;
        return false;
    }else if(reply->term < current_term) {
        return false;
    }
    if(_state != Leader) return false;            // 有可能处理过程中，leader编程follower
    if(!reply->success) {
        next_index[server_id]--;
        thread_pool::get().push([this, server_id, appended_num](){
            append_entries(server_id, appended_num);
        });
        return false;
    }

    // Success!
    match_index[server_id] =  std::max(match_index[server_id], args->prev_log_index +(int)args->entries.size());
    next_index[server_id] = match_index[server_id] + 1;
    *appended_num = *appended_num + 1;
    if(*appended_num > rpcs.size() / 2 ) {
        // 一半以上的服务器都成功了
        *appended_num = 0; 
        if(args->entries.size() > 0 && args->entries.back().term == current_term) {
            commit_index = std::max(commit_index, args->prev_log_index + (int)args->entries.size());
        }
    }
    return true;
}

std::shared_ptr<raft::rpc::append_entry_res> raft::raft_server::append_entries_handler(
    std::shared_ptr<rpc::append_entry_args> args) {
    
    std::unique_lock<std::mutex> _ul(m_mtx);
    std::shared_ptr<rpc::append_entry_res> res = std::make_shared<rpc::append_entry_res>();
    if(args->term < current_term) {
        res->term = current_term;
        res->success = false;
        persist();
        return res;
    }
    _state = Follower;
    if(args->term > current_term) {
        current_term = args->term;
        voted_for = -1;
    }

    last_reset_election_timeout = std::chrono::high_resolution_clock::now();
    if(args->prev_log_index > last_index) {
        res->term = current_term;
        res->success = false;
        persist();
        return res;
    }

    if(_log->get_entry(args->prev_log_index).term != args->prev_log_term) {
        res->term = current_term;
        res->success = false;
        persist();
        return res;
    }

    int index = args->prev_log_index + 1;
    for(; index < last_index + 1 && index < args->prev_log_index + 1 + args->entries.size(); ++index) {
        if(_log->get_entry(index).term != args->entries[index - args->prev_log_index - 1].term) {
            break;
        }
    }
    if(index != last_index + 1 && index != args->prev_log_index + 1 + args->entries.size()) {
        _log->del_entry_after(index);
        last_index = index - 1;
        for(int i = index - 1; i < args->prev_log_index + 1 + args->entries.size(); ++i) {
            _log->append_entry(args->entries[i - args->prev_log_index - 1]);
            ++last_index;
        }
    } else if(index == last_index + 1) {
            for(int i = index; i < args->prev_log_index + 1 + args->entries.size(); ++i) {
                _log->append_entry(args->entries[i - args->prev_log_index - 1]);
                ++last_index;
            }
    } else if(index == args->prev_log_index + 1 + args->entries.size()) {
        _log->del_entry_after(index);
    }
    
    if(args->leader_commit > commit_index) {
        commit_index = std::min(args->leader_commit, last_index);
    }

    res->term = current_term;
    res->success = true;
    persist();
    return res;
}