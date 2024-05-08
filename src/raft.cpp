#include "server.h"
#include <mutex>

void raft::server::do_heart_beat() {
    std::unique_lock<std::mutex> _ul(m_mtx);
    if(_state != Leader) return;
    
    for(int i = 0; i < rpcs.size(); i++) {
        if(i == _me) continue;
        if(next_index[i] <= snap_shot_index) {
            // 发送快照 to_do
            continue;
        }
        

    }
}