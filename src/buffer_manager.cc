#include "buffer_manager.h"

#include <iostream>

BufferManager::BufferManager() {}

BufferManager::BufferManager(StorageManager* storage_manager) {
  this->AttachStorageManager(storage_manager);
}

int BufferManager::FixPage(int page_id, int protection) { return 0; }
int BufferManager::FixNewPage() { return 0; }
int BufferManager::UnfixPage(int page_id) { return 0; }
// int BufferManager::NumFreeFrames() { return 0; }

// int BufferManager::Hash(int page_id) { return 0; }
// void BufferManager::RemoveCacheDescriptor(CacheDescriptor* pointer,
//                                           int page_id) {}
// void BufferManager::RemoveLRUElement(int frame_id) {}
// void BufferManager::SetDirty(int frame_id) {
//   this->frame_id_to_cache_pointer_[frame_id]->dirty = true;
// }
// void BufferManager::UnsetDirty(int frame_id) {
//   this->frame_id_to_cache_pointer_[frame_id]->dirty = true;
// }
// void BufferManager::WriteDirtys() {}
// void BufferManager::PrintFrame(int frame_id) {
//   std::cout << this->buffer_[frame_id].field << std::endl;
// }

void BufferManager::AttachStorageManager(StorageManager* storage_manager) {
  this->storage_manager_ = storage_manager;
}
