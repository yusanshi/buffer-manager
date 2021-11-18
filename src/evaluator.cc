#include "evaluator.h"

#include <iostream>
#include <vector>

Evaluator::Evaluator(std::string trace_filepath,
                     BufferManager* buffer_manager) {
  this->trace_file_.open(trace_filepath);
  if (!this->trace_file_.is_open()) {
    throw std::runtime_error("Failed to open trace file.");
  }

  this->buffer_manager_ = buffer_manager;
}

void Evaluator::Evaluate() {
  std::cout << "Running evaluation..." << std::endl;
  int mode, page_id;
  char delimiter;
  while ((this->trace_file_ >> mode >> delimiter >> page_id) &&
         (delimiter == ',')) {
    page_id -= 1;
    if (mode != 0 and mode != 1) {
      throw std::runtime_error("Bad trace format.");
    }
    int frame_id = this->buffer_manager_->FixPage(page_id, mode);
    if (mode == 0) {
      Page data = this->buffer_manager_->ReadFrame(frame_id);
    } else if (mode == 1) {
      Page data;
      this->buffer_manager_->WriteFrame(frame_id, data);
    }
    this->buffer_manager_->UnfixPage(page_id);
  }
}
