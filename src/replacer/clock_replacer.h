#ifndef CLOCK_REPLACER_H_
#define CLOCK_REPLACER_H_

#include <list>
#include <unordered_map>

#include "../common.h"
#include "replacer.h"

struct ClockCacheDescriptor : CacheDescriptor {};

class ClockReplacer : public Replacer {
 public:
  int GetVictim();
  void PolishPage(int page_id, int frame_id);
  void IncreasePinCount(int page_id);
  void DecreasePinCount(int page_id);

 private:
  std::unordered_map<int, ClockCacheDescriptor*> frame_id_to_cache_pointer_;
  std::unordered_map<int, ClockCacheDescriptor*> page2cache_;
  std::list<ClockCacheDescriptor> cache_list_;
};

#endif  // CLOCK_REPLACER_H_
