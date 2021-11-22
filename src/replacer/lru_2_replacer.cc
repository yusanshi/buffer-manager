#include "replacer/lru_2_replacer.h"

int LRU2Replacer::GetVictimFromSingle(
    std::list<LRU2CacheDescriptor *> &cache_list,
    std::unordered_map<int, std::list<LRU2CacheDescriptor *>::iterator>
        &page2cache) {
  for (auto i = cache_list.rbegin(); i != cache_list.rend(); ++i) {
    auto cache_descriptor = *i;
    if (cache_descriptor->pin_count == 0) {
      cache_list.erase(std::next(i).base());
      page2cache.erase(cache_descriptor->page_id);
      return cache_descriptor->page_id;
    }
  }
  return -1;
}

int LRU2Replacer::GetVictim() {
  {
    int page_id = this->GetVictimFromSingle(this->cold_cache_list_,
                                            this->page2cold_cache_);
    if (page_id != -1) {
      return page_id;
    }
  }
  {
    int page_id =
        this->GetVictimFromSingle(this->hot_cache_list_, this->page2hot_cache_);
    if (page_id != -1) {
      return page_id;
    }
  }

  throw std::runtime_error("Failed to get a victim frame.");
}

void LRU2Replacer::PolishPage(int page_id, int frame_id) {
  if (this->page2cold_cache_.find(page_id) != this->page2cold_cache_.end()) {
    // If in cold cache, polish it. Move to hot cache if necessary.
    LRU2CacheDescriptor *cache_descriptor;
    auto position = this->page2cold_cache_.at(page_id);
    this->cold_cache_list_.erase(position);
    cache_descriptor = *position;
    cache_descriptor->access_count += 1;
    if (cache_descriptor->access_count >= 2) {
      this->page2cold_cache_.erase(page_id);
      this->hot_cache_list_.push_front(cache_descriptor);
      this->page2hot_cache_[page_id] = this->hot_cache_list_.begin();
    } else {
      // If fact, for LRU-2, this block will never be executed :)
      this->cold_cache_list_.push_front(cache_descriptor);
      this->page2cold_cache_[page_id] = this->cold_cache_list_.begin();
    }
    return;
  }
  if (this->page2hot_cache_.find(page_id) != this->page2hot_cache_.end()) {
    // If in hot cache, polish it.
    LRU2CacheDescriptor *cache_descriptor;
    auto position = this->page2hot_cache_.at(page_id);
    this->hot_cache_list_.erase(position);
    cache_descriptor = *position;
    cache_descriptor->access_count += 1;
    this->hot_cache_list_.push_front(cache_descriptor);
    this->page2hot_cache_[page_id] = this->hot_cache_list_.begin();
    return;
  }

  // If not in both caches, create it and push front to cold cache
  LRU2CacheDescriptor *cache_descriptor;
  cache_descriptor = new LRU2CacheDescriptor();
  cache_descriptor->page_id = page_id;
  cache_descriptor->frame_id = frame_id;
  cache_descriptor->access_count += 1;
  this->cold_cache_list_.push_front(cache_descriptor);
  this->page2cold_cache_[page_id] = this->cold_cache_list_.begin();
}

void LRU2Replacer::IncreasePinCount(int page_id) {
  if (this->page2cold_cache_.find(page_id) != this->page2cold_cache_.end()) {
    (*this->page2cold_cache_[page_id])->pin_count += 1;
    return;
  }

  if (this->page2hot_cache_.find(page_id) != this->page2hot_cache_.end()) {
    (*this->page2hot_cache_[page_id])->pin_count += 1;
    return;
  }

  throw std::runtime_error("Page not found in both cold and hot caches.");
}

void LRU2Replacer::DecreasePinCount(int page_id) {
  if (this->page2cold_cache_.find(page_id) != this->page2cold_cache_.end()) {
    (*this->page2cold_cache_[page_id])->pin_count -= 1;
    return;
  }

  if (this->page2hot_cache_.find(page_id) != this->page2hot_cache_.end()) {
    (*this->page2hot_cache_[page_id])->pin_count -= 1;
    return;
  }

  throw std::runtime_error("Page not found in both cold and hot caches.");
}