#ifndef BUFFER_MANAGER_H_
#define BUFFER_MANAGER_H_

#include <array>
#include <atomic>
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
  std::atomic<int> hit_count_ = 0;
  std::atomic<int> miss_count_ = 0;
  std::array<std::mutex, NUM_PAGES> page_id_lock_;
  std::mutex buffer_lock_;
  std::mutex storage_manager_lock_;
  std::mutex replacer_lock_;
  std::mutex page_table_lock_;
  // Return a frame id for page `page_id`.
  // Insert the entry into the `page_table_` if not exists.
  int RequestFrame(int page_id);
  void ReportPerformance();
};

#endif  // BUFFER_MANAGER_H_
