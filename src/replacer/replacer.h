#ifndef REPLACER_H_
#define REPLACER_H_

class Replacer {
 public:
  // Select a victim frame and return its corresponding page id. Only the frames
  // with 0 `pin_count` can be victims. Throw error if all frames has non-zero
  // `pin_count`. Only use this when the page is not found in cache and the
  // buffer is full.
  // Some hooking operations for replacement policy should be done in this
  // function, i.e., `HookNotFoundFull`
  virtual int GetVictim(int page_id) = 0;
  // A hook function for replacement policy if the requested page is found in
  // cache.
  // Args : The page `page_id` is requested and the buffer manager returned
  // `frame_id`.
  virtual void HookFound(int page_id, int frame_id) = 0;
  // A hook function for replacement policy if the requested page is not found
  // and the buffer is not full.
  // Args : The page `page_id` is requested and the buffer manager returned
  // `frame_id`.
  virtual void HookNotFoundNotFull(int page_id, int frame_id) = 0;
  virtual void IncreasePinCount(int page_id) = 0;
  virtual void DecreasePinCount(int page_id) = 0;
};

#endif  // REPLACER_H_
