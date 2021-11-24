#include "parallel_buffer_manager.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "buffer_manager.h"

auto CurrentTime() { return std::chrono::steady_clock::now(); }

ParallelBufferPoolManager::ParallelBufferPoolManager(
    std::string storage_filepath, std::string replacement_policy,
    int num_threads)
    : storage_manager_(new StorageManager(storage_filepath)) {
  for (auto i = 0; i < num_threads; i++) {
    Replacer* replacer;
    int buffer_size =
        BUFFER_SIZE / num_threads + (i < (BUFFER_SIZE % num_threads) ? 1 : 0);
    if (replacement_policy == "LRU") {
      replacer = new LRUReplacer();
    } else if (replacement_policy == "LRU_2") {
      replacer = new LRU2Replacer();
    } else if (replacement_policy == "CLOCK") {
      replacer = new ClockReplacer(buffer_size);
    } else {
      throw std::logic_error("Invalid replacement policy.");
    }
    BufferManager* buffer_manager = new BufferManager(
        this->storage_manager_, replacer, buffer_size, i, num_threads);
    this->buffer_managers_.push_back(buffer_manager);
  }
}

ParallelBufferPoolManager::~ParallelBufferPoolManager() {
  for (auto buffer_manager : this->buffer_managers_) {
    delete buffer_manager;
  }
  this->ReportPerformance();
}

void ParallelBufferPoolManager::ReportPerformance() {
  int hit_count = 0;
  int miss_count = 0;
  for (auto buffer_manager : this->buffer_managers_) {
    auto pair = buffer_manager->GetStatistics();
    hit_count += pair.first;
    miss_count += pair.second;
  }
  std::cout << "Hit count: " << hit_count << "\tMiss count: " << miss_count
            << std::endl;
  std::cout << "Hit rate: "
            << static_cast<float>(hit_count) / (hit_count + miss_count)
            << std::endl;
  this->storage_manager_->ReportPerformance();
  std::cout << "Elapsed time: "
            << (std::chrono::duration_cast<std::chrono::microseconds>(
                    this->end_time_ - this->begin_time_)
                    .count()) /
                   1000000.0
            << "s" << std::endl;
}

void ParallelBufferPoolManager::Evaluate(std::string trace_filepath) {
  std::ifstream trace_file(trace_filepath);
  if (!trace_file.is_open()) {
    throw std::runtime_error("Failed to open trace file.");
  }
  std::vector<std::pair<int, int>> trace_data;
  int mode, page_id;
  char delimiter;
  while ((trace_file >> mode >> delimiter >> page_id) && (delimiter == ',')) {
    page_id -= 1;
    trace_data.push_back(std::make_pair(mode, page_id));
  }
  this->begin_time_ = CurrentTime();

  std::vector<std::thread> threads;
  for (auto buffer_manager : this->buffer_managers_) {
    threads.push_back(
        std::thread(&BufferManager::Evaluate, buffer_manager, trace_data));
  }
  for (auto& thread : threads) {
    thread.join();
  }

  this->end_time_ = CurrentTime();
}