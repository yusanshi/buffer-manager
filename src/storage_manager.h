#ifndef STORAGE_MANAGER_H_
#define STORAGE_MANAGER_H_

#include <fstream>
#include <string>

#include "common.h"

class StorageManager {
 public:
  StorageManager(std::string storage_filepath);
  ~StorageManager();
  Page ReadPage(int page_id);
  void WritePage(int page_id, Page page);

 private:
  std::fstream storage_file_;
  int num_pages_;
  int pages_[NUM_PAGES];
  int read_count_ = 0;
  int write_count_ = 0;
  void ReportPerformance();
};

#endif  // STORAGE_MANAGER_H_