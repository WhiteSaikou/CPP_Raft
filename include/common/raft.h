#pragma once
#include "log.h"
#include <unordered_map>
#include <memory>
#include "rpc.h"
#include <mutex>
namespace raft
{
    enum Sever_State {
        None = 0,
        Leader = 1,             // 领导
        Candidate = 2,          // 候选人
        Follower = 3,            // 跟随者

    };


    class raft_server {
      private:

        std::mutex m_mtx;
        Sever_State _state;
        // Stable parameter     需要在响应RPC之前，就更新到磁盘

        int current_term;        // 服务器已知最新的任期
        int voted_for;          // 当前任期内收到选票的 candidateId，如果没有投给任何候选人 则为空+
        // std::make_shared<log::log_manager> _log;
        std::shared_ptr<log::log_manager> _log;  // 日志条目；每个条目包含了用于状态机的命令，以及领导人接收到该条目时的任期（初始索引为1）

        // Volatile parameter   不需要更新到磁盘，易失性状态
        
        int commit_index;       // 已知已提交的最高的日志条目的索引（初始值为0，单调递增）
        int last_applied;       // 已经被应用到状态机的最高的日志条目的索引（初始值为0，单调递增）
        int snap_shot_index;
        int snap_shot_term;     // 快照中最后一条日志的id和term
        int last_index;         // 该节点日志中最新的日志索引
        int last_term;          // 该节点日志中最新的日志任期

        // VP of leader         Leader 服务器上的易失性状态

        std::vector<int> next_index;
        // 对于每一台服务器，发送到该服务器的下一个日志条目的索引（初始值为领导人最后的日志条目的索引+1）

        std::vector<int> match_index;
        // 对于每一台服务器，已知的已经复制到该服务器的最高日志条目的索引（初始值为0，单调递增）
        std::vector<std::shared_ptr<raft::rpc::raft_rpc>> rpcs;
        int _me; // 该服务器id

        std::chrono::_V2::system_clock::time_point last_heart_beat; // 上次心跳时间
        int heart_beat_timeout;

        std::chrono::_V2::system_clock::time_point last_reset_election_timeout; // 上次重置选举时间

        void persist();
      public:


        void leader_heart_beat(); // 循环检查是否需要心跳
        void do_heart_beat();
        bool append_entries(int server_id, std::shared_ptr<int> appended_num);
        std::shared_ptr<rpc::append_entry_res> append_entries_handler(std::shared_ptr<rpc::append_entry_args> args);
    };
};
