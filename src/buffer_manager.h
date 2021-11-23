#ifndef BUFFER_MANAGER_H_
#define BUFFER_MANAGER_H_

#include <array>
#include <mutex>
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
  // Read page from buffer. The page must be in buffer (by calling
  // `FixPage`)
  Page ReadPage(int page_id);
  // Write `page` to buffer and set the dirty flag. The page must be in buffer
  // (by calling `FixPage`)
  void WritePage(int page_id, Page page);
  // Pin the page in buffer.
  int FixPage(int page_id);
  // Unpin a in-buffer page.
  void UnfixPage(int page_id);

 private:
  std::array<Page, BUFFER_SIZE> buffer_;
  StorageManager* storage_manager_;
  Replacer* replacer_;
  // Map currently in-buffer page id to frame id and dirty flag.
  std::unordered_map<int, PageTable> page_table_;
  int hit_count_ = 0;
  int miss_count_ = 0;
  std::mutex mutex_;
  // Return a frame id for page `page_id`.
  // Insert the entry into the `page_table_` if not exists.
  int RequestFrame(int page_id);
  void ReportPerformance();
};

#endif  // BUFFER_MANAGER_H_
