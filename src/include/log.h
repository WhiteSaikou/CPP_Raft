#pragma once

#include <vector>
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
        };
        
        class log_manager {
            std::vector<log_entry> _logs;
        };
    }
};