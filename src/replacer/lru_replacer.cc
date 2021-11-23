#include "replacer/lru_replacer.h"

#include <iostream>

// Try to select a victim frame from back to front and move the frame to front
// (because it is now used by the new page)
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

// If found in cache, move the cache descriptor to front
void LRUReplacer::HookFound(int page_id, int frame_id) {
  this->cache_list_.splice(this->cache_list_.begin(), this->cache_list_,
                           this->page2cache_[page_id]);
}

// If not found in cache and cache is not full, create it and push front
void LRUReplacer::HookNotFoundNotFull(int page_id, int frame_id) {
  LRUCacheDescriptor* cache_descriptor;
  cache_descriptor = new LRUCacheDescriptor();
  cache_descriptor->page_id = page_id;
  cache_descriptor->frame_id = frame_id;
  this->cache_list_.push_front(cache_descriptor);
  this->page2cache_[page_id] = this->cache_list_.begin();
}