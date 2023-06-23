/**
 * To run this benchmark:
 * $ bazel run -c opt perf_puzzles/problems:unordered_map_for_range_loop
 *
 * You can find a solution to this puzzle in perf_puzzles/solutions folder.
 */

#include <benchmark/benchmark.h>

#include <cstdint>
#include <string>
#include <unordered_map>

volatile int64_t gSink = 0;

std::unordered_map<int64_t, std::string> buildData(int64_t size) {
  std::unordered_map<int64_t, std::string> data;
  data.reserve(size);
  for (int64_t i = 0; i < size; ++i) {
    data.insert({i, std::string(30, 'a')});
  }
  return data;
}

static void problem(benchmark::State& state) {
  std::unordered_map<int64_t, std::string> data = buildData(state.range(0));
// ==========================================================
// To start investigation without a compiler help
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrange-loop-construct"
  // ==========================================================
  for (auto _ : state) {
    for (const std::pair<int64_t, std::string>& item : data) {
      const std::string& value = item.second;
      gSink = value.size();
    }
  }
#pragma GCC diagnostic pop
}
BENCHMARK(problem)->Arg(1000);

BENCHMARK_MAIN();
