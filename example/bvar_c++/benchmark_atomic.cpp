//
// Created by Qihe Bian on 21/03/2018.
//

#include <iostream>
#include <atomic>
#include <gflags/gflags.h>
#include <bvar/bvar.h>
#include <thread>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <functional>


DEFINE_int32(thread_count, 1, "Thread count.");
DEFINE_int32(key_count, 100, "Unique key count.");
DEFINE_int32(duration, 30, "Duration for run in seconds.");

bool should_exit = false;
std::atomic<int64_t> work_func_count{};
std::unordered_map<std::string, std::unique_ptr<std::atomic<int64_t>>> funcs_count{};

void work_func(std::string func_name, int num) {
  funcs_count[func_name]->fetch_add(1);
  work_func_count.fetch_add(1);
}

void thread_func() {
  while (true && !should_exit) {
    std::vector<std::function<void(int)>> funcs;

    for (int i = 0; i < FLAGS_key_count && !should_exit; ++i) {
      std::stringstream ss;
      ss << "func" << std::setw(4) << std::setfill('0') << i;
      auto func = std::bind(work_func, ss.str(), std::placeholders::_1);
      funcs.emplace_back(func);
    }
    for (auto& func: funcs) {
      if (should_exit) {
        return;
      }
      int r = rand() % 1000;
      func(r);
    }
  }
}

int main(int argc, char *argv[]) {
  GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

  if (GFLAGS_NS::SetCommandLineOption("bvar_dump", "true").empty()) {
    std::cout << "Fail to enable ump dump" << std::endl;
  }

  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  for (int i = 0; i < FLAGS_key_count; ++i) {
    std::stringstream ss;
    ss << "func" << std::setw(4) << std::setfill('0') << i;
    funcs_count.emplace(std::make_pair(ss.str(), std::unique_ptr<std::atomic<int64_t>>(new std::atomic<int64_t>{})));
  }
  std::vector<std::thread> threads;
  for (int i = 0; i < FLAGS_thread_count; ++i) {
    threads.emplace_back(std::thread(thread_func));
  }
  while (true && !should_exit) {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    if (seconds >= FLAGS_duration) {
      should_exit = true;
    }
    usleep(1000);
  }
  for (auto& thread: threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  std::cout << "work_func: " << work_func_count.load() << std::endl;
  for (auto &p: funcs_count) {
    std::cout << p.first << ": " << p.second->load() << std::endl;
  }
  return 0;
}
