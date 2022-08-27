#include <benchmark/benchmark.h>
#include <rapidfuzz/distance/Levenshtein.hpp>
#include <string>
#include <vector>

template <typename T>
std::basic_string<T> str_multiply(std::basic_string<T> a, unsigned int b)
{
    std::basic_string<T> output;
    while (b--)
        output += a;

    return output;
}

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


static void BM_LevLongSimilarSequence(benchmark::State &state) {
  size_t len = state.range(0);
  size_t score_cutoff = state.range(1);
  std::string s1 = std::string("a") + str_multiply(std::string("b"), (len - 2)) + std::string("a");
  std::string s2 = str_multiply(std::string("b"), len);

  size_t num = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(rapidfuzz::levenshtein_distance(s1, s2, {1, 1, 1}, score_cutoff));
    ++num;
  }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num * len), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num * len),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

static void BM_LevLongNonSimilarSequence(benchmark::State &state) {
  size_t len = state.range(0);
  size_t score_cutoff = state.range(1);
  std::string s1 = str_multiply(std::string("a"), len);
  std::string s2 = str_multiply(std::string("b"), len);

  size_t num = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(rapidfuzz::levenshtein_distance(s1, s2, {1, 1, 1}, score_cutoff));
    ++num;
  }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num * len), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num * len),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(BM_LevLongSimilarSequence)
    ->Args({100, 30})
    ->Args({500, 30})
    ->Args({5000, 30})
    ->Args({10000, 30})
    ->Args({20000, 30})
    ->Args({50000, 30});

BENCHMARK(BM_LevLongNonSimilarSequence)
    ->Args({100, 30})
    ->Args({500, 30})
    ->Args({5000, 30})
    ->Args({10000, 30})
    ->Args({20000, 30})
    ->Args({50000, 30});

BENCHMARK(BM_LevWeightedDist1);
BENCHMARK(BM_LevWeightedDist2);

BENCHMARK(BM_LevNormWeightedDist1);
BENCHMARK(BM_LevNormWeightedDist2);


BENCHMARK_MAIN();