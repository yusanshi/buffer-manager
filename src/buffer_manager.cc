#include "buffer_manager.h"

#include <iostream>

BufferManager::BufferManager(StorageManager* storage_manager,
                             Replacer* replacer)
    : storage_manager_(storage_manager), replacer_(replacer) {}

BufferManager::~BufferManager() {
  std::cout << "Flush dirty frames ..." << std::endl;
  for (auto i : this->page_table_) {
    auto page_id = i.first;
    auto [frame_id, dirty] = i.second;
    if (dirty) {
      this->storage_manager_->WritePage(page_id, this->buffer_.at(frame_id));
    }
  }
  this->ReportPerformance();

  delete this->storage_manager_;
  delete this->replacer_;
}

Page BufferManager::ReadPage(int page_id) {
  std::scoped_lock lock(this->page_id_lock_.at(page_id));
  int frame_id;
  {
    std::scoped_lock lock(this->page_table_lock_);
    frame_id = this->page_table_.at(page_id).frame_id;
  }
  Page page;
  {
    std::scoped_lock lock(this->buffer_lock_);
    page = this->buffer_.at(frame_id);
  }
  return page;
}

void BufferManager::WritePage(int page_id, Page page) {
  std::scoped_lock lock(this->page_id_lock_.at(page_id));
  int frame_id;
  {
    std::scoped_lock lock(this->page_table_lock_);
    this->page_table_.at(page_id).dirty = true;
    frame_id = this->page_table_.at(page_id).frame_id;
  }
  {
    std::scoped_lock lock(this->buffer_lock_);
    this->buffer_.at(frame_id) = page;
  }
}

int BufferManager::FixPage(int page_id) {
  std::scoped_lock lock(this->page_id_lock_.at(page_id));
  int frame_id = this->RequestFrame(page_id);
  {
    std::scoped_lock lock(this->replacer_lock_);
    this->replacer_->IncreasePinCount(page_id);
  }
  return frame_id;
}

void BufferManager::UnfixPage(int page_id) {
  std::scoped_lock lock(this->page_id_lock_.at(page_id));
  {
    std::scoped_lock lock(this->replacer_lock_);
    this->replacer_->DecreasePinCount(page_id);
  }
}

int BufferManager::RequestFrame(int page_id) {
  // If found in cache
  bool condition;
  {
    std::scoped_lock lock(this->page_table_lock_);
    condition = this->page_table_.find(page_id) != this->page_table_.end();
  }
  if (condition) {
    this->hit_count_++;

    if (VERBOSE) {
      std::cout << "Page " << page_id << " found in cache" << std::endl;
    }
    int frame_id;
    {
      std::scoped_lock lock(this->page_table_lock_);
      frame_id = this->page_table_.at(page_id).frame_id;
    }
    {
      std::scoped_lock lock(this->replacer_lock_);
      this->replacer_->HookFound(page_id, frame_id);
    }
    return frame_id;
  }

  this->miss_count_++;

  // If not found in cache and buffer is not full, allocate a new frame
  int page_table_size;
  {
    std::scoped_lock lock(this->page_table_lock_);
    page_table_size = this->page_table_.size();
  }
  int buffer_size;
  {
    std::scoped_lock lock(this->buffer_lock_);
    buffer_size = this->buffer_.size();
  }
  if (page_table_size < buffer_size) {
    if (VERBOSE) {
      std::cout << "Page " << page_id
                << " not found in cache, allocate a new frame in buffer"
                << std::endl;
    }
    for (auto i = page_id % buffer_size;
         i < page_id % buffer_size + buffer_size; i++) {
      auto j = i % buffer_size;
      bool condition;
      {
        std::scoped_lock lock(this->page_table_lock_);
        condition = this->page_table_.find(j) == this->page_table_.end();
      }
      if (condition) {
        int frame_id = j;

        Page page;
        {
          std::scoped_lock lock(this->storage_manager_lock_);
          page = this->storage_manager_->ReadPage(page_id);
        }
        {
          std::scoped_lock lock(this->buffer_lock_);
          this->buffer_.at(frame_id) = page;
        }
        {
          std::scoped_lock lock(this->page_table_lock_);
          this->page_table_[page_id] = PageTable{frame_id, false};
        }
        {
          std::scoped_lock lock(this->replacer_lock_);
          this->replacer_->HookNotFoundNotFull(page_id, frame_id);
        }
        return frame_id;
      }
    }
  }

  // If not found and buffer is full, try to get a victim frame ...
  int victim_page_id;
  {
    std::scoped_lock lock(this->replacer_lock_);
    victim_page_id = this->replacer_->GetVictim(page_id);
  }

  if (VERBOSE) {
    std::cout << "Page " << page_id
              << " not found in cache and buffer is full, found a victim frame"
              << std::endl;
  }
  int victim_frame_id;
  bool victim_dirty;
  {
    std::scoped_lock lock(this->page_table_lock_);
    auto page_table = this->page_table_.at(victim_page_id);
    victim_frame_id = page_table.frame_id;
    victim_dirty = page_table.dirty;
  }

  if (victim_dirty) {
    Page page;
    {
      std::scoped_lock lock(this->buffer_lock_);
      page = this->buffer_.at(victim_frame_id);
    }

    {
      std::scoped_lock lock(this->storage_manager_lock_);
      this->storage_manager_->WritePage(victim_page_id, page);
    }
  }
  Page page;
  {
    std::scoped_lock lock(this->storage_manager_lock_);
    page = this->storage_manager_->ReadPage(page_id);
  }
  {
    std::scoped_lock lock(this->buffer_lock_);
    this->buffer_.at(victim_frame_id) = page;
  }
  {
    std::scoped_lock lock(this->page_table_lock_);
    this->page_table_.erase(victim_page_id);
    this->page_table_[page_id] = PageTable{victim_frame_id, false};
  }
  return victim_frame_id;
}

void BufferManager::ReportPerformance() {
  std::cout << "Hit count: " << this->hit_count_
            << "\tMiss count: " << this->miss_count_ << std::endl;
  std::cout << "Hit rate: "
            << static_cast<float>(this->hit_count_) /
                   (this->hit_count_ + this->miss_count_)
            << std::endl;
}