#include "replacer/clock_replacer.h"

int ClockReplacer::GetVictim() {
  if (!full_) {
    throw std::runtime_error("Buffer should be full.");
  }
  for (auto i = 0; i < 2 * this->cache_list_.size(); i++) {
    auto cache_descriptor = *this->pointer_;
    this->pointer_ = this->NextPointer();
    if (cache_descriptor->pin_count == 0) {
      if (cache_descriptor->referenced) {
        cache_descriptor->referenced = false;
      } else {
        cache_descriptor->referenced = true;
        this->page2cache_.erase(cache_descriptor->page_id);
        return cache_descriptor->page_id;
      }
    }
  }
  throw std::runtime_error("Failed to get a victim frame.");
}

void ClockReplacer::PolishPage(int page_id, int frame_id) {
  if (this->page2cache_.find(page_id) == this->page2cache_.end()) {
    if (this->full_) {
      this->page2cache_[page_id] = this->PrevPointer();
      (*this->page2cache_[page_id])->page_id = page_id;
    } else {
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
        this->full_ = true;
        this->pointer_ = this->cache_list_.begin();
      }
    }
  }
}

void ClockReplacer::IncreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count += 1;
}

void ClockReplacer::DecreasePinCount(int page_id) {
  (*this->page2cache_[page_id])->pin_count -= 1;
}

std::list<ClockCacheDescriptor *>::iterator ClockReplacer::NextPointer() {
  auto next = std::next(this->pointer_);
  if (next == this->cache_list_.end()) {
    return this->cache_list_.begin();
  }
  return next;
}
std::list<ClockCacheDescriptor *>::iterator ClockReplacer::PrevPointer() {
  if (this->pointer_ == this->cache_list_.begin()) {
    return std::prev(this->cache_list_.end());
  }
  return std::prev(this->pointer_);
}