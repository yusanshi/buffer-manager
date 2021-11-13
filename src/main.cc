#include <iostream>

#include "buffer_manager/clock_buffer_manager.h"
#include "buffer_manager/lru_2_buffer_manager.h"
#include "buffer_manager/lru_buffer_manager.h"
#include "common.h"
#include "evaluator.h"
#include "storage_manager.h"

int main() {
  StorageManager storage_manager(STORAGE_FILEPATH);
  BufferManager* buffer_manager;
  if (REPLACEMENT_POLICY == ReplacementPolicy::LRU) {
    buffer_manager = new LRUBufferManager();
  } else if (REPLACEMENT_POLICY == ReplacementPolicy::LRU_2) {
    buffer_manager = new LRU2BufferManager();
  } else if (REPLACEMENT_POLICY == ReplacementPolicy::CLOCK) {
    buffer_manager = new ClockBufferManager();
  } else {
    throw std::logic_error("Invalid replacement policy.");
  }
  buffer_manager->AttachStorageManager(&storage_manager);
  Evaluator evaluator(TRACE_FILEPATH, buffer_manager);
  evaluator.Evaluate();
}
