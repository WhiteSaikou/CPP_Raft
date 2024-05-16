#pragma once

#include <unordered_map>
#include <vector>
#include "common.h"

namespace raft {
    namespace log {
        enum OPTION {
            NONE = 0,
            SET = 1,
            DEL = 2,
            GET = 3,
        };

        struct log_entry_func
        {
            OPTION op;
            int key;
            int value;
        };

        struct log_entry
        {
            int index;
            int term;
            log_entry_func content;

            log_entry():index(-1){}
        };
        
        class log_manager {
          private:
            std::vector<log_entry> _logs;
          public:
            const log_entry& get_entry(int index) { 
                if(index < _logs.size()) {
                    return _logs[index];
                }
                return log_entry();
                }

            // 会删除index之后的所有日志，包括index
            void del_entry_after(int index) {
                
                if(index >= _logs.size() || index < 0) return;
                _logs.resize(index);
            }

            void append_entry(const log_entry& entry) {
                ASSERT(entry.index == _logs.size(), "index must be equal to the size of logs");
                _logs.push_back(entry);
            }
        };
    };
};