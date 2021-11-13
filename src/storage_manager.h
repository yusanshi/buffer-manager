#ifndef STORAGE_MANAGER_H_
#define STORAGE_MANAGER_H_

#include <fstream>
#include <string>

#include "common.h"

class StorageManager {
 public:
  StorageManager(std::string storage_filepath);
  FrameOrPage ReadPage(int page_id);
  int WritePage(int page_id, FrameOrPage page);
  int Seek(int offset, int position);
  void IncreaseNumPages();
  int GetNumPages();
  void SetUse(int index, int use_bit);
  int GetUse(int index);

 private:
  std::fstream current_file_;
  int num_pages_;
  int pages_[NUM_PAGES];
};

#endif  // STORAGE_MANAGER_H_