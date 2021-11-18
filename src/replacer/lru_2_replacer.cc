#include "lru_2_replacer.h"

std::tuple<int, int, bool> LRU2Replacer::GetVictim() { return {0, 0, false}; }

void LRU2Replacer::SetDirty(int page_id) {}

std::vector<int> LRU2Replacer::GetDirtyPages() {
  std::vector<int> dirty_pages;
  return dirty_pages;
}

void LRU2Replacer::PolishPage(int page_id, int frame_id) {}

void LRU2Replacer::IncreasePinCount(int page_id) {}

void LRU2Replacer::DecreasePinCount(int page_id) {}