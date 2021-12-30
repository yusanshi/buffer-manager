#include <iostream>

#include "common.h"
#include "cxxopts.h"
#include "parallel_buffer_manager.h"

int main(int argc, char** argv) {
  cxxopts::Options options("Buffer Manager", "Buffer Manager");
  options.add_options()("r,replacement",
                        "Replacement policy, choose among [LRU, LRU_2, CLOCK]",
                        cxxopts::value<std::string>()->default_value("LRU"))(
      "t,threads", "Thread number",
      cxxopts::value<int>()->default_value("1"))("h,help", "Print usage");
  auto result = options.parse(argc, argv);
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    std::exit(EXIT_SUCCESS);
  }
  std::string replacement_policy = result["replacement"].as<std::string>();
  int num_threads = result["threads"].as<int>();
  ParallelBufferManager parallel_buffer_manager(
      STORAGE_FILEPATH, replacement_policy, num_threads);
  parallel_buffer_manager.Evaluate(TRACE_FILEPATH);
}
