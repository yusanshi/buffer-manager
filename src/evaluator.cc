#include "evaluator.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

auto CurrentTime() { return std::chrono::steady_clock::now(); }

Evaluator::Evaluator(std::string trace_filepath, BufferManager* buffer_manager,
                     int num_threads)
    : buffer_manager_(buffer_manager), num_threads_(num_threads) {
  std::ifstream trace_file(trace_filepath);
  if (!trace_file.is_open()) {
    throw std::runtime_error("Failed to open trace file.");
  }
  int mode, page_id;
  char delimiter;
  while ((trace_file >> mode >> delimiter >> page_id) && (delimiter == ',')) {
    page_id -= 1;
    this->trace_file_data_.push_back(std::make_pair(mode, page_id));
  }
  this->begin_time_ = CurrentTime();
}

Evaluator::~Evaluator() {
  auto end_time = CurrentTime();
  std::cout << "Elapsed time: "
            << (std::chrono::duration_cast<std::chrono::microseconds>(
                    end_time - this->begin_time_)
                    .count()) /
                   1000000.0
            << "s" << std::endl;
}

void Evaluator::Evaluate() {
  std::vector<std::thread> threads;
  for (int rank = 0; rank < this->num_threads_; rank++) {
    threads.push_back(
        std::thread(&Evaluator::EvaluateSingleThread, this, rank));
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

void Evaluator::EvaluateSingleThread(int rank) {
  // Use `std::cout << "1" + "2"` instead of `std::cout << "1" << "2"` in
  // multithreading settings to avoid interweaved outputs
  std::cout << "[" + std::to_string(rank) + "] Running evaluation...\n";
  for (int i = rank; i < this->trace_file_data_.size();
       i += this->num_threads_) {
    auto [mode, page_id] = this->trace_file_data_[i];
    this->buffer_manager_->FixPage(page_id);
    if (mode == 0) {
      Page data = this->buffer_manager_->ReadPage(page_id);
    } else if (mode == 1) {
      Page data;
      this->buffer_manager_->WritePage(page_id, data);
    } else {
      throw std::runtime_error("Bad trace format.");
    }
    this->buffer_manager_->UnfixPage(page_id);
  }
}
