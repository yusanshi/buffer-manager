#include "evaluator.h"

#include <iostream>
#include <vector>

Evaluator::Evaluator(std::string trace_filepath,
                     BufferManager* buffer_manager) {
  this->current_file_.open(trace_filepath);
  this->buffer_manager_ = buffer_manager;
}

void Evaluator::Evaluate() {
  std::cout << "Running evaluation..." << std::endl;
  int mode, page_id;
  char delimiter;
  while ((this->current_file_ >> mode >> delimiter >> page_id) &&
         (delimiter == ',')) {
    page_id -= 1;
    if (mode == 0) {
      //   std::cout << "Reading page " << page_id << std::endl;
      //   this->storage_manager_->ReadPage(page_id);
    } else if (mode == 1) {
      //   std::cout << "Writing page " << page_id << std::endl;
      //   this->storage_manager_->WritePage(0, 0);
    } else {
      throw std::runtime_error("Bad format.");
    }
  }
}
