#include "replacer/clock_replacer.h"

int ClockReplacer::GetVictim(int page_id) {
  for (auto i = 0; i < 2 * this->cache_list_.size(); i++) {
    auto cache_descriptor = *this->pointer_;
    this->SpinPointer();
    if (cache_descriptor->pin_count == 0) {
      if (cache_descriptor->referenced) {
        cache_descriptor->referenced = false;
      } else {
        int victim_page_id = cache_descriptor->page_id;
        cache_descriptor->referenced = true;
        cache_descriptor->page_id = page_id;
        this->page2cache_[page_id] = this->page2cache_[victim_page_id];
        this->page2cache_.erase(victim_page_id);
        return victim_page_id;
      }
    }
  }
  throw std::runtime_error("Failed to get a victim frame.");
}

void ClockReplacer::PostHookFound(int page_id, int frame_id) {}

void ClockReplacer::PostHookNotFoundNotFull(int page_id, int frame_id) {
  ClockCacheDescriptor *cache_descriptor;
  cache_descriptor = new ClockCacheDescriptor();
  cache_descriptor->page_id = page_id;
  cache_descriptor->frame_id = frame_id;
  // false for better performance
  cache_descriptor->referenced = false;
  //   this->cache_list_.push_back(cache_descriptor);
  //   this->page2cache_[page_id] = std::prev(this->cache_list_.end());
  this->cache_list_.push_front(cache_descriptor);
  this->page2cache_[page_id] = this->cache_list_.begin();

  if (this->cache_list_.size() == BUFFER_SIZE) {
    this->pointer_ = this->cache_list_.begin();
  }
}

void ClockReplacer::IncreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count += 1;
}

void ClockReplacer::DecreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count -= 1;
}

void ClockReplacer::SpinPointer() {
  auto next = std::next(this->pointer_);
  if (next == this->cache_list_.end()) {
    this->pointer_ = this->cache_list_.begin();
    return;
  }
  this->pointer_ = next;
}
