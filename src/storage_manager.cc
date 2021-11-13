#include "storage_manager.h"

#include <fstream>

StorageManager::StorageManager(std::string storage_filepath) {
  this->current_file_.open(storage_filepath,
                           std::ios::in | std::ios::out | std::ios::binary);
}

FrameOrPage StorageManager::ReadPage(int page_id) {
  FrameOrPage page;
  return page;
}

int StorageManager::WritePage(int page_id, FrameOrPage page) { return 0; }
int StorageManager::Seek(int offset, int position) { return 0; }
void StorageManager::IncreaseNumPages() {}
int StorageManager::GetNumPages() { return 0; }
void StorageManager::SetUse(int index, int use_bit) {}
int StorageManager::GetUse(int index) { return 0; }