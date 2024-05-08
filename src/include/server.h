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
        Folower = 3,            // 跟随者

    };


    class server {
      private:

        std::mutex m_mtx;
        Sever_State _state;
        // Stable parameter     需要在响应RPC之前，就更新到磁盘

        int currentTerm;        // 服务器已知最新的任期
        int voted_for;          // 当前任期内收到选票的 candidateId，如果没有投给任何候选人 则为空
        log::log_manager _log;  // 日志条目；每个条目包含了用于状态机的命令，以及领导人接收到该条目时的任期（初始索引为1）

        // Volatile parameter   不需要更新到磁盘，易失性状态
        
        int commit_index;       // 已知已提交的最高的日志条目的索引（初始值为0，单调递增）
        int last_applied;       // 已经被应用到状态机的最高的日志条目的索引（初始值为0，单调递增）
        int snap_shot_index;
        int snap_shot_term;     // 快照中最后一条日志的id和term

        // VP of leader         Leader 服务器上的易失性状态

        std::vector<int> next_index;
        // 对于每一台服务器，发送到该服务器的下一个日志条目的索引（初始值为领导人最后的日志条目的索引+1）

        std::vector<int,int> match_index;
        // 对于每一台服务器，已知的已经复制到该服务器的最高日志条目的索引（初始值为0，单调递增）
        std::vector<std::shared_ptr<raft::rpc::raft_rpc>> rpcs;
        int _me; // 该服务器id



      public:
        // 对上层提供接口
            // 当上层执行一条命令的时候，会先调用do_work，返回true代表可以执行，执行成功调用commit
        bool do_work(log::log_entry_func func);

        void leader_heart_beat(); // 循环检查是否需要心跳

        void do_heart_beat();
    };
};
