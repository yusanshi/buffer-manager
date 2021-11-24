#include "replacer/lru_2_replacer.h"

int LRU2Replacer::GetVictim(int page_id) {
  // First, try to select a victim frame from back to front from cold cache and
  // move the frame to front (because it is now used by the new page)
  for (auto i = this->cold_cache_list_.rbegin();
       i != this->cold_cache_list_.rend(); ++i) {
    auto cache_descriptor = *i;
    if (cache_descriptor->pin_count == 0) {
      int victim_page_id = cache_descriptor->page_id;
      cache_descriptor->page_id = page_id;
      cache_descriptor->access_count = 1;
      this->cold_cache_list_.splice(this->cold_cache_list_.begin(),
                                    this->cold_cache_list_,
                                    this->page2cold_cache_.at(victim_page_id));
      this->page2cold_cache_[page_id] =
          this->page2cold_cache_.at(victim_page_id);
      this->page2cold_cache_.erase(victim_page_id);
      return victim_page_id;
    }
  }

  // Then try to do the same for hot cache, except that the frame should be
  // moved to the front of cold cache.
  for (auto i = this->hot_cache_list_.rbegin();
       i != this->hot_cache_list_.rend(); ++i) {
    auto cache_descriptor = *i;
    if (cache_descriptor->pin_count == 0) {
      int victim_page_id = cache_descriptor->page_id;
      cache_descriptor->page_id = page_id;
      cache_descriptor->access_count = 1;
      this->cold_cache_list_.splice(this->cold_cache_list_.begin(),
                                    this->hot_cache_list_,
                                    this->page2hot_cache_.at(victim_page_id));
      this->page2cold_cache_[page_id] =
          this->page2hot_cache_.at(victim_page_id);
      this->page2hot_cache_.erase(victim_page_id);
      return victim_page_id;
    }
  }

  throw std::runtime_error("Failed to get a victim frame.");
}

void LRU2Replacer::HookFound(int page_id, int frame_id) {
  // If in cold cache, move to front of cold list, or to hot cache if necessary.
  if (this->page2cold_cache_.find(page_id) != this->page2cold_cache_.end()) {
    (*this->page2cold_cache_.at(page_id))->access_count += 1;
    if ((*this->page2cold_cache_.at(page_id))->access_count >= 2) {
      this->hot_cache_list_.splice(this->hot_cache_list_.begin(),
                                   this->cold_cache_list_,
                                   this->page2cold_cache_.at(page_id));
      this->page2hot_cache_[page_id] = this->page2cold_cache_.at(page_id);
      this->page2cold_cache_.erase(page_id);
    } else {
      // If fact, for LRU-2, this block will never be executed :)
      this->cold_cache_list_.splice(this->cold_cache_list_.begin(),
                                    this->cold_cache_list_,
                                    this->page2cold_cache_.at(page_id));
    }

    return;
  }

  // If in hot cache, move to front of hot list
  if (this->page2hot_cache_.find(page_id) != this->page2hot_cache_.end()) {
    (*this->page2hot_cache_.at(page_id))->access_count += 1;
    this->hot_cache_list_.splice(this->hot_cache_list_.begin(),
                                 this->hot_cache_list_,
                                 this->page2hot_cache_.at(page_id));

    return;
  }

  throw std::runtime_error("Page not found in both cold and hot caches.");
}

// If not found in both caches and caches are not full, create it and push front
// to cold cache
void LRU2Replacer::HookNotFoundNotFull(int page_id, int frame_id) {
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
    (*this->page2cold_cache_.at(page_id))->pin_count += 1;
    return;
  }

  if (this->page2hot_cache_.find(page_id) != this->page2hot_cache_.end()) {
    (*this->page2hot_cache_.at(page_id))->pin_count += 1;
    return;
  }

  throw std::runtime_error("Page not found in both cold and hot caches.");
}

void LRU2Replacer::DecreasePinCount(int page_id) {
  if (this->page2cold_cache_.find(page_id) != this->page2cold_cache_.end()) {
    (*this->page2cold_cache_.at(page_id))->pin_count -= 1;
    return;
  }

  if (this->page2hot_cache_.find(page_id) != this->page2hot_cache_.end()) {
    (*this->page2hot_cache_.at(page_id))->pin_count -= 1;
    return;
  }

  throw std::runtime_error("Page not found in both cold and hot caches.");
}