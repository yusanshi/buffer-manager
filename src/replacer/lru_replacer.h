#ifndef LRU_REPLACER_H_
#define LRU_REPLACER_H_

#include <list>
#include <unordered_map>

#include "../common.h"  //TODO: include relative or absolute path?
#include "replacer.h"

class LRUReplacer : public Replacer {
 public:
  std::tuple<int, int, bool> GetVictim();
  void SetDirty(int page_id);
  ;
  std::vector<int> GetDirtyPages();
  void PolishPage(int page_id, int frame_id);
  void IncreasePinCount(int page_id);
  void DecreasePinCount(int page_id);

 private:
  std::unordered_map<int, std::list<CacheDescriptor *>::iterator> page2cache_;
  std::list<CacheDescriptor *> cache_list_;
};

#endif  // LRU_REPLACER_H_
