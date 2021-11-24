#ifndef STORAGE_MANAGER_H_
#define STORAGE_MANAGER_H_

#include <atomic>
#include <fstream>
#include <mutex>
#include <string>

#include "common.h"

class StorageManager {
 public:
  StorageManager(std::string storage_filepath);
  // Read page `page_id` from storage file.
  Page ReadPage(int page_id);
  // Write page `page` to storage file at `page_id`.
  void WritePage(int page_id, Page page);
  void ReportPerformance();

 private:
  std::fstream storage_file_;
  std::atomic<int> read_count_ = 0;
  std::atomic<int> write_count_ = 0;
  std::mutex mutex_;
};

#endif  // STORAGE_MANAGER_H_