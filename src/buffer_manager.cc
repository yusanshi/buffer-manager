#include "buffer_manager.h"

#include <iostream>

BufferManager::BufferManager(StorageManager* storage_manager,
                             Replacer* replacer) {
  this->storage_manager_ = storage_manager;
  this->replacer_ = replacer;
}

BufferManager::~BufferManager() {
  std::cout << "Flush dirty frames ..." << std::endl;
  for (auto i : this->page_table_) {
    auto page_id = i.first;
    auto [frame_id, dirty] = i.second;
    if (dirty) {
      this->storage_manager_->WritePage(page_id, this->buffer_[frame_id]);
    }
  }
  this->ReportPerformance();
}

Page BufferManager::ReadFrame(int frame_id) { return this->buffer_[frame_id]; }
void BufferManager::WriteFrame(int frame_id, Page page) {
  this->buffer_[frame_id] = page;
}

int BufferManager::FixPage(int page_id, bool dirty) {
  int frame_id = this->RequestFrame(page_id, dirty);
  this->replacer_->PolishPage(page_id, frame_id);
  this->replacer_->IncreasePinCount(page_id);
  return frame_id;
}

void BufferManager::UnfixPage(int page_id) {
  this->replacer_->DecreasePinCount(page_id);
}

int BufferManager::RequestFrame(int page_id, bool dirty) {
  // If find in cache
  if (this->page_table_.find(page_id) != this->page_table_.end()) {
    this->hit_count_ += 1;
    std::cout << "Page " << page_id << " found in cache" << std::endl;
    this->page_table_.at(page_id).dirty =
        dirty || this->page_table_.at(page_id).dirty;
    int frame_id = this->page_table_.at(page_id).frame_id;
    return frame_id;
  }
  this->miss_count_ += 1;

  // If buffer is not full, allocate a new frame
  int buffer_size = this->buffer_.size();
  if (this->page_table_.size() < buffer_size) {
    std::cout << "Page " << page_id
              << " not found in cache, allocate a new frame in buffer"
              << std::endl;
    for (auto i = page_id % buffer_size;
         i < page_id % buffer_size + buffer_size; i++) {
      auto j = i % buffer_size;
      if (this->page_table_.find(j) == this->page_table_.end()) {
        int frame_id = j;
        this->buffer_[frame_id] = this->storage_manager_->ReadPage(page_id);
        this->page_table_[page_id] = PageTable{frame_id, dirty};
        return frame_id;
      }
    }
  }

  // Finally, try to get a victim frame ...
  int victim_page_id = this->replacer_->GetVictim();
  if (victim_page_id != -1) {
    std::cout << "Page " << page_id
              << " not found in cache and buffer is full, found a victim frame"
              << std::endl;
    auto [victim_frame_id, victim_dirty] = this->page_table_.at(victim_page_id);
    if (victim_dirty) {
      this->storage_manager_->WritePage(victim_page_id,
                                        this->buffer_[victim_frame_id]);
    }
    this->buffer_[victim_frame_id] = this->storage_manager_->ReadPage(page_id);
    this->page_table_.erase(victim_page_id);
    this->page_table_[page_id] = PageTable{victim_frame_id, dirty};
    return victim_frame_id;
  }
  throw std::runtime_error("Failed to get a victim frame.");
}

void BufferManager::ReportPerformance() {
  std::cout << "Buffer manager:" << std::endl;
  std::cout << "Hit count:" << this->hit_count_ << std::endl;
  std::cout << "Miss count:" << this->miss_count_ << std::endl;
  std::cout << "Hit rate:"
            << static_cast<float>(this->hit_count_) /
                   (this->hit_count_ + this->miss_count_)
            << std::endl;
}