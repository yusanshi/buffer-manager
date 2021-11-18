#include <iostream>

#include "buffer_manager.h"
#include "common.h"
#include "cxxopts.h"
#include "evaluator.h"
#include "replacer/clock_replacer.h"
#include "replacer/lru_2_replacer.h"
#include "replacer/lru_replacer.h"
#include "storage_manager.h"

int main(int argc, char** argv) {
  cxxopts::Options options("Storage Buffer Manager", "Storage Buffer Manager");
  options.add_options()("r,replacement",
                        "Replacement policy, choose among [LRU, LRU_2, CLOCK]",
                        cxxopts::value<std::string>()->default_value("LRU"))(
      "h,help", "Print usage");
  auto result = options.parse(argc, argv);
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    std::exit(EXIT_SUCCESS);
  }
  StorageManager* storage_manager = new StorageManager(STORAGE_FILEPATH);
  std::string replacement_policy = result["replacement"].as<std::string>();
  Replacer* replacer;
  if (replacement_policy == "LRU") {
    replacer = new LRUReplacer();
  } else if (replacement_policy == "LRU_2") {
    replacer = new LRU2Replacer();
  } else if (replacement_policy == "CLOCK") {
    replacer = new ClockReplacer();
  } else {
    throw std::logic_error("Invalid replacement policy.");
  }
  BufferManager* buffer_manager = new BufferManager(storage_manager, replacer);

  Evaluator evaluator(TRACE_FILEPATH, buffer_manager);
  evaluator.Evaluate();
  delete buffer_manager;
  delete storage_manager;
}
