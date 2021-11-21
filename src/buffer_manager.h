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
  // Return frame in buffer.
  Page ReadFrame(int frame_id);
  // Write `page` to buffer.
  void WriteFrame(int frame_id, Page page);
  // Request a frame from the buffer and pin it.
  int FixPage(int page_id, bool dirty);
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
  // Return a frame id for page `page_id`.
  // Insert the entry into the `page_table_` if not exists.
  // Set `dirty` flag in `page_table_`.
  int RequestFrame(int page_id, bool dirty);
  void ReportPerformance();
};

#endif  // BUFFER_MANAGER_H_
