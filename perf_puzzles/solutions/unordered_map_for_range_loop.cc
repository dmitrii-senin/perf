/**
 * To run this benchmark:
 * $ bazel run -c opt perf_puzzles/solutions:unordered_map_for_range_loop
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
  for (auto _ : state) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrange-loop-construct"
    for (const std::pair<int64_t, std::string>& item : data) {
      gSink = item.second.size();
    }
  }
#pragma GCC diagnostic pop
}
BENCHMARK(problem)->Arg(1000);

/**
 * Return type of a dereferenced map iterator is "std::pair<const Key, Value>&".
 * If we don't provide this "const" in front of the "Key" we'll get unnecessary
 * copy (from std::pair<const Key, Value) to std::pair<Key, Value>) of each item
 * in the container while we iterate through it.
 */
static void solution1(benchmark::State& state) {
  std::unordered_map<int64_t, std::string> data = buildData(state.range(0));
  for (auto _ : state) {
    for (const std::pair<const int64_t, std::string>& item : data) {
      //                 ^^^^^
      // this "const" is necessary to prevent a copy creation
      const std::string& value = item.second;
      gSink = value.size();
    }
  }
}
BENCHMARK(solution1)->Arg(1000);

static void solution2(benchmark::State& state) {
  std::unordered_map<int64_t, std::string> data = buildData(state.range(0));
  for (auto _ : state) {
    for (const auto& item : data) {
      //       ^^^^^
      // "const auto&" also works:
      // "auto" will be deduced to "std::pair<const Key, Value>" type
      gSink = item.second.size();
    }
  }
}
BENCHMARK(solution2)->Arg(1000);

BENCHMARK_MAIN();
