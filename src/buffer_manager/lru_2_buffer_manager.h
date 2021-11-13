#ifndef LRU_2_BUFFER_MANAGER_H_
#define LRU_2_BUFFER_MANAGER_H_

#include <list>
#include <unordered_map>

#include "../buffer_manager.h"
#include "../common.h"

class LRU2BufferManager : public BufferManager {
 public:
  int SelectVictim();  // TODO

 private:
  std::unordered_map<int, CacheDescriptor*> frame_id_to_cache_pointer_;
  std::unordered_map<int, CacheDescriptor*> page_id_to_cache_pointer_;
  std::list<CacheDescriptor> cache_list;
};

#endif  // LRU_2_BUFFER_MANAGER_H_
