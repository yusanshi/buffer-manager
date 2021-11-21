#ifndef STORAGE_MANAGER_H_
#define STORAGE_MANAGER_H_

#include <fstream>
#include <string>

#include "common.h"

class StorageManager {
 public:
  StorageManager(std::string storage_filepath);
  ~StorageManager();
  // Read page `page_id` from storage file.
  Page ReadPage(int page_id);
  // Write page `page` to storage file at `page_id`.
  void WritePage(int page_id, Page page);

 private:
  std::fstream storage_file_;
  int read_count_ = 0;
  int write_count_ = 0;
  void ReportPerformance();
};

#endif  // STORAGE_MANAGER_H_