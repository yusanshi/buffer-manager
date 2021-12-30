#ifndef BUFFER_MANAGER_GROUP_H_
#define BUFFER_MANAGER_GROUP_H_

#include <array>
#include <unordered_map>
#include <vector>

#include "buffer_manager.h"
#include "common.h"
#include "replacer/clock_replacer.h"
#include "replacer/lru_2_replacer.h"
#include "replacer/lru_replacer.h"
#include "replacer/replacer.h"
#include "storage_manager.h"

class ParallelBufferManager {
 public:
  ParallelBufferManager(std::string storage_filepath,
                        std::string replacement_policy, int num_threads);
  ~ParallelBufferManager();
  void Evaluate(std::string trace_filepath);

 private:
  std::vector<BufferManager*> buffer_managers_;
  StorageManager* storage_manager_;
  std::chrono::steady_clock::time_point begin_time_;
  std::chrono::steady_clock::time_point end_time_;
  void ReportPerformance();
};

#endif  // BUFFER_MANAGER_GROUP_H_
