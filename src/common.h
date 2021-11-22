#ifndef COMMON_H_
#define COMMON_H_

#define PAGE_SIZE 4096
#define BUFFER_SIZE 1024
#define NUM_PAGES 50000
#define STORAGE_FILEPATH "data/data.dbf"
#define TRACE_FILEPATH "data/data-5w-50w-zipf.txt"
// Set to true to do real reading/writing from/to storage
#define PROCESS_IO false

struct Page {
  char field[PAGE_SIZE];
};

struct CacheDescriptor {
  int frame_id;
  int page_id;
  bool latch = false;
  int pin_count = 0;
};

#endif  // COMMON_H_