#include "replacer/lru_replacer.h"

#include <iostream>

int LRUReplacer::GetVictim() {
  for (auto i = this->cache_list_.rbegin(); i != this->cache_list_.rend();
       ++i) {
    auto cache_descriptor = *i;
    if (cache_descriptor->pin_count == 0) {
      this->cache_list_.erase(std::next(i).base());
      this->page2cache_.erase(cache_descriptor->page_id);
      return cache_descriptor->page_id;
    }
  }
  return -1;
}

void LRUReplacer::IncreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count += 1;
}

void LRUReplacer::DecreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count -= 1;
}

void LRUReplacer::PolishPage(int page_id, int frame_id) {
  LRUCacheDescriptor* cache_descriptor;
  if (this->page2cache_.find(page_id) == this->page2cache_.end()) {
    // If not in LRU list, create it and push front (later)
    cache_descriptor = new LRUCacheDescriptor();
    cache_descriptor->page_id = page_id;
    cache_descriptor->frame_id = frame_id;
  } else {
    // Else, remove it and push front (later)
    auto position = this->page2cache_.at(page_id);
    this->cache_list_.erase(position);
    cache_descriptor = *position;
  }
  // Move to front and create the map
  this->cache_list_.push_front(cache_descriptor);
  this->page2cache_[page_id] = this->cache_list_.begin();
}