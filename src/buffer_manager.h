#ifndef BUFFER_MANAGER_H_
#define BUFFER_MANAGER_H_

#include "common.h"
#include "storage_manager.h"

class BufferManager {
 public:
  BufferManager();
  BufferManager(StorageManager* storage_manager);
  int FixPage(int page_id, int protection);
  int FixNewPage();
  int UnfixPage(int page_id);
  // int NumFreeFrames();

  virtual int SelectVictim() = 0;
  // int Hash(int page_id);
  // void RemoveCacheDescriptor(CacheDescriptor* pointer, int page_id);
  // void RemoveLRUElement(int frame_id);
  // void SetDirty(int frame_id);
  // void UnsetDirty(int frame_id);
  // void WriteDirtys();
  // void PrintFrame(int frame_id);
  void AttachStorageManager(StorageManager* storage_manager);
  virtual ~BufferManager() {}

 private:
  FrameOrPage buffer_[BUFFER_SIZE];
  StorageManager* storage_manager_;
};

#endif  // BUFFER_MANAGER_H_
