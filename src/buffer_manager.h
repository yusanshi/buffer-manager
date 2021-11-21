#ifndef BUFFER_MANAGER_H_
#define BUFFER_MANAGER_H_

#include <array>
#include <unordered_map>

#include "common.h"
#include "replacer/replacer.h"
#include "storage_manager.h"

struct PageTable {
  int frame_id;
  bool dirty;
};

class BufferManager {
 public:
  BufferManager(StorageManager* storage_manager, Replacer* replacer);
  ~BufferManager();
  Page ReadFrame(int frame_id);
  void WriteFrame(int frame_id, Page page);
  int FixPage(int page_id, bool dirty);
  void UnfixPage(int page_id);

 private:
  std::array<Page, BUFFER_SIZE> buffer_;
  StorageManager* storage_manager_;
  Replacer* replacer_;
  std::unordered_map<int, PageTable> page_table_;
  int hit_count_ = 0;
  int miss_count_ = 0;
  int RequestFrame(int page_id, bool dirty);
  void ReportPerformance();
};

#endif  // BUFFER_MANAGER_H_
