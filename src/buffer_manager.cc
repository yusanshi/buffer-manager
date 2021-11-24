#include "buffer_manager.h"

#include <iostream>

BufferManager::BufferManager(StorageManager* storage_manager,
                             Replacer* replacer, int buffer_size, int rank,
                             int num_thread)
    : storage_manager_(storage_manager),
      replacer_(replacer),
      buffer_size_(buffer_size),
      rank_(rank),
      num_threads_(num_thread),
      buffer_(new Page[buffer_size]) {}

BufferManager::~BufferManager() {
  std::cout << "[" << this->rank_ << "] Flush dirty frames ..." << std::endl;
  for (auto i : this->page_table_) {
    auto page_id = i.first;
    auto [frame_id, dirty] = i.second;
    if (dirty) {
      this->storage_manager_->WritePage(page_id, this->buffer_[frame_id]);
    }
  }

  delete this->replacer_;
}

Page BufferManager::ReadPage(int page_id) {
  int frame_id = this->page_table_[page_id].frame_id;
  return this->buffer_[frame_id];
}

void BufferManager::WritePage(int page_id, Page page) {
  this->page_table_[page_id].dirty = true;
  int frame_id = this->page_table_[page_id].frame_id;
  this->buffer_[frame_id] = page;
}

int BufferManager::FixPage(int page_id) {
  int frame_id = this->RequestFrame(page_id);
  this->replacer_->IncreasePinCount(page_id);
  return frame_id;
}

void BufferManager::UnfixPage(int page_id) {
  this->replacer_->DecreasePinCount(page_id);
}

int BufferManager::RequestFrame(int page_id) {
  // If found in cache
  if (this->page_table_.find(page_id) != this->page_table_.end()) {
    this->hit_count_ += 1;
    if (VERBOSE) {
      std::cout << "Page " << page_id << " found in cache" << std::endl;
    }
    int frame_id = this->page_table_[page_id].frame_id;
    this->replacer_->HookFound(page_id, frame_id);
    return frame_id;
  }
  this->miss_count_ += 1;

  // If not found in cache and buffer is not full, allocate a new frame
  if (this->page_table_.size() < this->buffer_size_) {
    if (VERBOSE) {
      std::cout << "Page " << page_id
                << " not found in cache, allocate a new frame in buffer"
                << std::endl;
    }

    for (auto i = page_id % this->buffer_size_;
         i < page_id % this->buffer_size_ + this->buffer_size_; i++) {
      auto j = i % this->buffer_size_;
      if (this->page_table_.find(j) == this->page_table_.end()) {
        int frame_id = j;
        this->buffer_[frame_id] = this->storage_manager_->ReadPage(page_id);
        this->page_table_[page_id] = PageTable{frame_id, false};
        this->replacer_->HookNotFoundNotFull(page_id, frame_id);
        return frame_id;
      }
    }
  }

  // If not found and buffer is full, try to get a victim frame ...
  int victim_page_id = this->replacer_->GetVictim(page_id);
  if (VERBOSE) {
    std::cout << "Page " << page_id
              << " not found in cache and buffer is full, found a victim frame"
              << std::endl;
  }
  auto [victim_frame_id, victim_dirty] = this->page_table_[victim_page_id];
  if (victim_dirty) {
    this->storage_manager_->WritePage(victim_page_id,
                                      this->buffer_[victim_frame_id]);
  }
  this->buffer_[victim_frame_id] = this->storage_manager_->ReadPage(page_id);
  this->page_table_.erase(victim_page_id);
  this->page_table_[page_id] = PageTable{victim_frame_id, false};
  return victim_frame_id;
}

void BufferManager::Evaluate(
    const std::vector<std::pair<int, int>>& trace_data) {
  std::cout << "[" << this->rank_ << "] Running evaluation...\n";
  for (auto [mode, page_id] : trace_data) {
    if (page_id % this->num_threads_ != this->rank_) {
      continue;
    }
    this->FixPage(page_id);
    if (mode == 0) {
      Page data = this->ReadPage(page_id);
    } else if (mode == 1) {
      Page data;
      this->WritePage(page_id, data);
    } else {
      throw std::runtime_error("Bad trace format.");
    }
    this->UnfixPage(page_id);
  }
}
std::pair<int, int> BufferManager::GetStatistics() {
  return std::make_pair(this->hit_count_, this->miss_count_);
}