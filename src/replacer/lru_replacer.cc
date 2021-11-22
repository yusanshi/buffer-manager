#include "replacer/lru_replacer.h"

#include <iostream>

int LRUReplacer::GetVictim(int page_id) {
  for (auto i = this->cache_list_.rbegin(); i != this->cache_list_.rend();
       ++i) {
    auto cache_descriptor = *i;
    if (cache_descriptor->pin_count == 0) {
      int victim_page_id = cache_descriptor->page_id;
      cache_descriptor->page_id = page_id;
      this->cache_list_.splice(this->cache_list_.begin(), this->cache_list_,
                               this->page2cache_[victim_page_id]);
      this->page2cache_[page_id] = this->page2cache_[victim_page_id];
      this->page2cache_.erase(victim_page_id);
      return victim_page_id;
    }
  }
  throw std::runtime_error("Failed to get a victim frame.");
}

void LRUReplacer::IncreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count += 1;
}

void LRUReplacer::DecreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count -= 1;
}

void LRUReplacer::PostHookFound(int page_id, int frame_id) {
  this->cache_list_.splice(this->cache_list_.begin(), this->cache_list_,
                           this->page2cache_[page_id]);
}

void LRUReplacer::PostHookNotFoundNotFull(int page_id, int frame_id) {
  // If not in LRU list, create it and push front
  LRUCacheDescriptor* cache_descriptor;
  cache_descriptor = new LRUCacheDescriptor();
  cache_descriptor->page_id = page_id;
  cache_descriptor->frame_id = frame_id;
  this->cache_list_.push_front(cache_descriptor);
  this->page2cache_[page_id] = this->cache_list_.begin();
}