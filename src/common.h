#ifndef COMMON_H_
#define COMMON_H_

#define FRAME_OR_PAGE_SIZE 4096
#define BUFFER_SIZE 1024
#define NUM_PAGES 50000
#define STORAGE_FILEPATH "data.dbf"
#define TRACE_FILEPATH "data/data-5w-50w-zipf.txt"
#define REPLACEMENT_POLICY ReplacementPolicy::LRU

struct FrameOrPage {
  char field[FRAME_OR_PAGE_SIZE];
};

struct CacheDescriptor {
  int frame_id;
  int page_id;
  bool latch;
  int pin_count;
  bool dirty;
};

enum class ReplacementPolicy { LRU, LRU_2, CLOCK };

#endif  // COMMON_H_