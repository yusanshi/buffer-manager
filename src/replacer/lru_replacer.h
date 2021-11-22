#ifndef LRU_REPLACER_H_
#define LRU_REPLACER_H_

#include <list>
#include <unordered_map>

#include "common.h"
#include "replacer.h"

struct LRUCacheDescriptor : CacheDescriptor {};

class LRUReplacer : public Replacer {
 public:
  int GetVictim(int page_id);
  void HookFound(int page_id, int frame_id);
  void HookNotFoundNotFull(int page_id, int frame_id);
  void IncreasePinCount(int page_id);
  void DecreasePinCount(int page_id);

 private:
  std::unordered_map<int, std::list<LRUCacheDescriptor *>::iterator>
      page2cache_;
  std::list<LRUCacheDescriptor *> cache_list_;
};

#endif  // LRU_REPLACER_H_
