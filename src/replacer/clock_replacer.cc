#include "clock_replacer.h"

std::tuple<int, int, bool> ClockReplacer::GetVictim() { return {0, 0, false}; }

void ClockReplacer::SetDirty(int page_id) {}

std::vector<int> ClockReplacer::GetDirtyPages() {
  std::vector<int> dirty_pages;
  return dirty_pages;
}

void ClockReplacer::PolishPage(int page_id, int frame_id) {}

void ClockReplacer::IncreasePinCount(int page_id) {}

void ClockReplacer::DecreasePinCount(int page_id) {}