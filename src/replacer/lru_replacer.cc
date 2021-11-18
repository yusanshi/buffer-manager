#include "lru_replacer.h"

#include <iostream>

std::tuple<int, int, bool> LRUReplacer::GetVictim() {
  for (auto i = this->cache_list_.rbegin(); i != this->cache_list_.rend();
       ++i) {
    auto cache_descriptor = *i;
    if (cache_descriptor->pin_count == 0) {
      this->cache_list_.pop_back();
      this->page2cache_.erase(cache_descriptor->page_id);
      return {cache_descriptor->frame_id, cache_descriptor->page_id,
              cache_descriptor->dirty};
    }
  }
  return {-1, -1, false};
}

void LRUReplacer::SetDirty(int page_id) {
  (*this->page2cache_[page_id])->dirty = true;
}

std::vector<int> LRUReplacer::GetDirtyPages() {
  std::vector<int> dirty_pages;
  for (auto i = this->cache_list_.begin(); i != this->cache_list_.end(); ++i) {
    auto cache_descriptor = *i;
    if (cache_descriptor->dirty) {
      dirty_pages.push_back(cache_descriptor->page_id);
    }
  }
  return dirty_pages;
}

void LRUReplacer::IncreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count += 1;
}

void LRUReplacer::DecreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count -= 1;
}

void LRUReplacer::PolishPage(int page_id, int frame_id) {
  CacheDescriptor* cache_descriptor;
  if (this->page2cache_.find(page_id) == this->page2cache_.end()) {
    // If not in LRU list, create
    cache_descriptor = new CacheDescriptor();
    cache_descriptor->page_id = page_id;
    cache_descriptor->frame_id = frame_id;
  } else {
    // If in the list, remove it
    auto position = this->page2cache_.at(page_id);
    this->cache_list_.erase(position);
    cache_descriptor = *position;
  }
  // Move to front and create the map
  this->cache_list_.push_front(cache_descriptor);
  this->page2cache_[page_id] = this->cache_list_.begin();
}