#include <benchmark/benchmark.h>
#include <rapidfuzz/distance/Levenshtein.hpp>
#include <string>
#include <vector>

// Define another benchmark
static void BM_LevWeightedDist1(benchmark::State &state) {
  std::string a = "aaaaa aaaaa";
  for (auto _ : state) {
    benchmark::DoNotOptimize(rapidfuzz::levenshtein_distance(a, a));
  }
  state.SetLabel("Similar Strings");
}

static void BM_LevWeightedDist2(benchmark::State &state) {
  std::string a = "aaaaa aaaaa";
  std::string b = "bbbbb bbbbb";
  for (auto _ : state) {
    benchmark::DoNotOptimize(rapidfuzz::levenshtein_distance(a, b));
  }
  state.SetLabel("Different Strings");
}

static void BM_LevNormWeightedDist1(benchmark::State &state) {
  std::string a = "aaaaa aaaaa";
  for (auto _ : state) {
    benchmark::DoNotOptimize(rapidfuzz::levenshtein_normalized_distance(a, a));
  }
  state.SetLabel("Similar Strings");
}

static void BM_LevNormWeightedDist2(benchmark::State &state) {
  std::string a = "aaaaa aaaaa";
  std::string b = "bbbbb bbbbb";
  for (auto _ : state) {
    benchmark::DoNotOptimize(rapidfuzz::levenshtein_normalized_distance(a, b));
  }
  state.SetLabel("Different Strings");
}


BENCHMARK(BM_LevWeightedDist1);
BENCHMARK(BM_LevWeightedDist2);

BENCHMARK(BM_LevNormWeightedDist1);
BENCHMARK(BM_LevNormWeightedDist2);


BENCHMARK_MAIN();