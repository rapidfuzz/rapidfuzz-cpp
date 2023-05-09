#include <benchmark/benchmark.h>
#include <random>
#include <rapidfuzz/distance/Jaro.hpp>
#include <string>
#include <vector>

std::string generate(int max_length)
{
    std::string possible_characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(possible_characters.size() - 1));
    std::string ret = "";
    for (int i = 0; i < max_length; i++) {
        int random_index = dist(engine);
        ret += possible_characters[static_cast<size_t>(random_index)];
    }
    return ret;
}

template <typename T>
std::basic_string<T> str_multiply(std::basic_string<T> a, unsigned int b)
{
    std::basic_string<T> output;
    while (b--)
        output += a;

    return output;
}

static void BM_JaroLongSimilarSequence(benchmark::State& state)
{
    size_t len = state.range(0);
    size_t score_cutoff = state.range(1);
    std::string s1 = std::string("a") + str_multiply(std::string("b"), (len - 2)) + std::string("a");
    std::string s2 = str_multiply(std::string("b"), len);

    size_t num = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rapidfuzz::jaro_similarity(s1, s2));
        ++num;
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num * len), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num * len),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

static void BM_JaroLongNonSimilarSequence(benchmark::State& state)
{
    size_t len = state.range(0);
    size_t score_cutoff = state.range(1);
    std::string s1 = str_multiply(std::string("a"), len);
    std::string s2 = str_multiply(std::string("b"), len);

    size_t num = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rapidfuzz::jaro_similarity(s1, s2));
        ++num;
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num * len), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num * len),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(BM_JaroLongSimilarSequence)
    ->Args({100, 30})
    ->Args({500, 30})
    ->Args({5000, 30})
    ->Args({10000, 30})
    ->Args({20000, 30})
    ->Args({50000, 30});

BENCHMARK(BM_JaroLongNonSimilarSequence)
    ->Args({100, 30})
    ->Args({500, 30})
    ->Args({5000, 30})
    ->Args({10000, 30})
    ->Args({20000, 30})
    ->Args({50000, 30});

BENCHMARK_MAIN();