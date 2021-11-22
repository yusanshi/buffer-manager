#ifndef CLOCK_REPLACER_H_
#define CLOCK_REPLACER_H_

#include <list>
#include <unordered_map>

#include "common.h"
#include "replacer.h"

struct ClockCacheDescriptor : CacheDescriptor {
  bool referenced = true;
};

class ClockReplacer : public Replacer {
 public:
  int GetVictim();
  void PolishPage(int page_id, int frame_id);
  void IncreasePinCount(int page_id);
  void DecreasePinCount(int page_id);

 private:
  std::unordered_map<int, std::list<ClockCacheDescriptor *>::iterator>
      page2cache_;
  std::list<ClockCacheDescriptor *> cache_list_;
  std::list<ClockCacheDescriptor *>::iterator pointer_;
  bool full_ = false;
  std::list<ClockCacheDescriptor *>::iterator NextPointer();
  std::list<ClockCacheDescriptor *>::iterator PrevPointer();
};

#endif  // CLOCK_REPLACER_H_
