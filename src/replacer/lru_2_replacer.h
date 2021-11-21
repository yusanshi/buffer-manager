#ifndef LRU_2_REPLACER_H_
#define LRU_2_REPLACER_H_

#include <list>
#include <unordered_map>

#include "common.h"
#include "replacer.h"

struct LRU2CacheDescriptor : CacheDescriptor {};

class LRU2Replacer : public Replacer {
 public:
  int GetVictim();
  void PolishPage(int page_id, int frame_id);
  void IncreasePinCount(int page_id);
  void DecreasePinCount(int page_id);

 private:
  std::unordered_map<int, LRU2CacheDescriptor*> frame_id_to_cache_pointer_;
  std::unordered_map<int, LRU2CacheDescriptor*> page2cache_;
  std::list<LRU2CacheDescriptor> cache_list_;
};

#endif  // LRU_2_REPLACER_H_
