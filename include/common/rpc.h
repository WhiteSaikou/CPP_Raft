#pragma once
#include <vector>
#include "log.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "binaryTo.h"
#include <memory>
namespace raft {

    namespace rpc {
        
        
        

        struct append_entry_args
        {
          public:
            int term;               // 领导人的任期
            int leader_id;          // 领导人ID 便于客户端重定向
            int prev_log_index;     // 新日志条目的上一条目的索引
            int prev_log_term;      // 新日志条目的上一条目的任期
            int leader_commit;      // leader已知的最高已提交的日志条目索引
            std::vector<log::log_entry> entries;

            append_entry_args(int _term, int _leader_id, int _p_index, int _p_term, int _leader_commit):
            term(_term), leader_id(_leader_id), prev_log_index(_p_index), prev_log_term(_p_term), leader_commit(leader_commit) {}

            void serialize_to_buf(char*& buf) {
                binary::serialize_to_buf(term, buf);
                binary::serialize_to_buf(leader_id, buf);
                binary::serialize_to_buf(prev_log_index, buf);
                binary::serialize_to_buf(prev_log_term, buf);
                binary::serialize_to_buf(leader_commit, buf);
                binary::serialize_to_buf(entries, buf);
            }

            void deserialize_from_buf(char*& buf) {
                binary::deserialize_from_buf(term, buf);
                binary::deserialize_from_buf(leader_id, buf);
                binary::deserialize_from_buf(prev_log_index, buf);
                binary::deserialize_from_buf(prev_log_term, buf);
                binary::deserialize_from_buf(leader_commit, buf);
                binary::deserialize_from_buf(entries, buf);
            }
            
        };

        struct append_entry_res
        {
            int term;               // 当前任期 对于leader而言，他会更新自己的的任期（？）
            bool success;            // 如果follower所含有的条目和 prevLogIndex 以及 prevLogTerm 匹配上了，则为 true

            void serialize_to_buf(char*& buf) {
                binary::serialize_to_buf(term, buf);
                binary::serialize_to_buf(success, buf);
            }

            void deserialize_from_buf(char*& buf) {
                binary::deserialize_from_buf(term, buf);
                binary::deserialize_from_buf(success, buf);
            }
        };
        
        struct req_vote_args
        {
            int term;               // 候选人任期号
            int candidateId;        // 候选人ID
            int lastLogIndex;       // 候选人最后的日志条目的索引值
            int lastLogTerm;        // 候选人最后的日志条目的任期号

            void serialize_to_buf(char*& buf) {
                binary::serialize_to_buf(term, buf);
                binary::serialize_to_buf(candidateId, buf);
                binary::serialize_to_buf(lastLogIndex, buf);
                binary::serialize_to_buf(lastLogTerm, buf);
            }

            void deserialize_from_buf(char*& buf) {
                binary::deserialize_from_buf(term, buf);
                binary::deserialize_from_buf(candidateId, buf);
                binary::deserialize_from_buf(lastLogIndex, buf);
                binary::deserialize_from_buf(lastLogTerm, buf);
            }
        };

        using req_vote_res = append_entry_res;
        
        
        
        struct rpc_node
        {
            unsigned int id;
            std::string addr;
            int port;
            int socket_fd;
        };
        
        class raft_rpc {
          private:
            // to do
          public:
            std::shared_ptr<append_entry_res> request(std::shared_ptr<append_entry_args>);
            std::shared_ptr<req_vote_res> request(std::shared_ptr<append_entry_args>);
        };
    }
};