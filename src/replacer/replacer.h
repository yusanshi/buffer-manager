#ifndef REPLACER_H_
#define REPLACER_H_

#include <vector>

class Replacer {
 public:
  virtual std::tuple<int, int, bool> GetVictim() = 0;
  virtual void SetDirty(int page_id) = 0;
  virtual std::vector<int> GetDirtyPages() = 0;
  virtual void PolishPage(int page_id, int frame_id) = 0;
  virtual void IncreasePinCount(int page_id) = 0;
  virtual void DecreasePinCount(int page_id) = 0;
};

#endif  // REPLACER_H_
