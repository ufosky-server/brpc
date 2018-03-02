//
// Created by ufo on 18-3-2.
//

#include <iostream>
#include <gflags/gflags.h>
#include <bvar/bvar.h>
#include <thread>

bvar::Adder<int> int_adder;
bvar::Adder<int> int_adder2;
bvar::Adder<double> double_adder;
bvar::Maxer<int> int_maxer;
bvar::Miner<int> int_miner;
bvar::IntRecorder int_recorder;
bvar::LatencyRecorder latency_recorder("latency_recorder");
bvar::Window<bvar::Adder<int>> window(&int_adder, 10);
bvar::PerSecond<bvar::Adder<int> > per_second(&int_adder2, 60);

void work_func() {
  for (int i = 0; i < 10000; ++i) {
    int_adder << 1;
    int_adder2 << 1;
    double_adder << 2.0;
    int r = rand() % 100;
    int_maxer << r;
    int_miner << r;
    int_recorder << r;
    latency_recorder << r;
  }
}

int main() {
  if (gflags::SetCommandLineOption("bvar_dump", "true").empty()) {
    std::cout << "Fail to enable bvar dump" << std::endl;
  }

  int_adder.expose("int_adder");
  int_adder2.expose("int_adder2");
  double_adder.expose("double_adder");
  int_maxer.expose("int_maxer");
  int_miner.expose("int_miner");
  int_recorder.expose("int_recorder");
  latency_recorder.expose("latency_recorder");
  window.expose("window");
  per_second.expose("per_second");

  std::vector<std::thread> threads;
  for (int i = 0; i < 100; ++i) {
    threads.push_back(std::thread(work_func));
  }
  for (auto &thread: threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  while (true) {
    usleep(10000);
  }
  return 0;
}
