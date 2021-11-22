#ifndef REPLACER_H_
#define REPLACER_H_

class Replacer {
 public:
  // Select a victim frame and return its corresponding page id. Only the frames
  // with 0 `pin_count` can be victims. Throw error if all frames has non-zero
  // `pin_count`. Only use this when the buffer is full.
  virtual int GetVictim(int page_id) = 0;
  // When the page `page_id` is requested and the buffer manager has return
  // `frame_id`, the method will be called. It's a hook function for
  // replacemennt policy.
  virtual void PostHookFound(int page_id, int frame_id) = 0;
  virtual void PostHookNotFoundNotFull(int page_id, int frame_id) = 0;
  virtual void IncreasePinCount(int page_id) = 0;
  virtual void DecreasePinCount(int page_id) = 0;
};

#endif  // REPLACER_H_
