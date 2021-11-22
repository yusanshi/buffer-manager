#ifndef LRU_2_REPLACER_H_
#define LRU_2_REPLACER_H_

#include <list>
#include <unordered_map>

#include "common.h"
#include "replacer.h"

struct LRU2CacheDescriptor : CacheDescriptor {
  int access_count = 0;
};

class LRU2Replacer : public Replacer {
 public:
  int GetVictim(int page_id);
  void HookFound(int page_id, int frame_id);
  void HookNotFoundNotFull(int page_id, int frame_id);
  void IncreasePinCount(int page_id);
  void DecreasePinCount(int page_id);

 private:
  std::unordered_map<int, std::list<LRU2CacheDescriptor *>::iterator>
      page2cold_cache_;
  std::unordered_map<int, std::list<LRU2CacheDescriptor *>::iterator>
      page2hot_cache_;
  std::list<LRU2CacheDescriptor *> cold_cache_list_;
  std::list<LRU2CacheDescriptor *> hot_cache_list_;
};

#endif  // LRU_2_REPLACER_H_
