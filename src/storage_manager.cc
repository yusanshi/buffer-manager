#include "storage_manager.h"

#include <filesystem>
#include <fstream>
#include <iostream>

StorageManager::StorageManager(std::string storage_filepath) {
  if (!std::filesystem::exists(storage_filepath)) {
    std::cout << "Storage file not exists, create new ..." << std::endl;
    std::ofstream storage_file(storage_filepath);
    storage_file.close();
  }
  int target_size = NUM_PAGES * FRAME_OR_PAGE_SIZE;
  if (std::filesystem::file_size(storage_filepath) != target_size) {
    std::cout << "Storage file size is wrong, refill ..." << std::endl;
    std::ofstream storage_file(
        storage_filepath, std::ios::out | std::ios::binary | std::ios::trunc);
    auto memory_block = new char[target_size];
    storage_file.write(memory_block, target_size);
    storage_file.close();
  }
  this->storage_file_.open(storage_filepath,
                           std::ios::in | std::ios::out | std::ios::binary);

  if (!this->storage_file_.is_open()) {
    throw std::runtime_error("Failed to open storage file.");
  }
}

StorageManager::~StorageManager() { this->ReportPerformance(); }

Page StorageManager::ReadPage(int page_id) {
  std::cout << "Reading page " << page_id << " from storage file" << std::endl;
  this->read_count_ += 1;
  Page page;
  this->storage_file_.seekg(FRAME_OR_PAGE_SIZE * page_id, std::ios::beg);
  this->storage_file_.read(page.field, FRAME_OR_PAGE_SIZE);
  return page;
}

void StorageManager::WritePage(int page_id, Page page) {
  // TODO is page changed? use &?
  std::cout << "Writing page " << page_id << " to storage file" << std::endl;
  this->write_count_ += 1;
  this->storage_file_.seekp(FRAME_OR_PAGE_SIZE * page_id, std::ios::beg);
  this->storage_file_.write(page.field, FRAME_OR_PAGE_SIZE);
}

void StorageManager::ReportPerformance() {
  std::cout << "Storage manager:" << std::endl;
  std::cout << "Read count:" << this->read_count_ << std::endl;
  std::cout << "Write count:" << this->write_count_ << std::endl;
}