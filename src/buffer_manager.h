#ifndef BUFFER_MANAGER_H_
#define BUFFER_MANAGER_H_

#include <array>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "replacer/replacer.h"
#include "storage_manager.h"

struct PageTable {
  int frame_id;
  bool dirty;
};

class BufferManager {
 public:
  BufferManager(StorageManager* storage_manager, Replacer* replacer,
                int buffer_size, int rank, int num_thread);
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
  // Evaluate on `trace_data`. Note `trace_data` is the overall trace data and
  // each instance of BufferManager will run part of them (if `page_id %
  // num_threads == rank`)
  void Evaluate(const std::vector<std::pair<int, int>>& trace_data);
  // Return <hit_count, miss_count>
  std::pair<int, int> GetStatistics();

 private:
  Page* buffer_;
  int buffer_size_;
  int rank_;
  int num_threads_;
  StorageManager* storage_manager_;
  Replacer* replacer_;
  // Map currently in-buffer page id to frame id and dirty flag.
  std::unordered_map<int, PageTable> page_table_;
  int hit_count_ = 0;
  int miss_count_ = 0;
  // Return a frame id for page `page_id`.
  // Insert the entry into the `page_table_` if not exists.
  int RequestFrame(int page_id);
};

#endif  // BUFFER_MANAGER_H_
