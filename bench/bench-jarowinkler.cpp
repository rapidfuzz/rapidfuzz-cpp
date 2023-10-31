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

#ifdef RAPIDFUZZ_SIMD
template <size_t MaxLen1, size_t MaxLen2>
static void BM_Jaro_SIMD(benchmark::State& state)
{
    std::vector<std::string> seq1;
    std::vector<std::string> seq2;
    std::vector<double> results(64);
    for (int i = 0; i < 64; i++)
        seq1.push_back(generate(MaxLen1));
    for (int i = 0; i < 10000; i++)
        seq2.push_back(generate(MaxLen2));

    size_t num = 0;
    for (auto _ : state) {
        rapidfuzz::experimental::MultiJaro<MaxLen1> scorer(seq1.size());
        for (const auto& str1 : seq1)
            scorer.insert(str1);

        for (const auto& str2 : seq2)
            scorer.similarity(&results[0], results.size(), str2);

        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}
#endif

template <size_t MaxLen1, size_t MaxLen2>
static void BM_Jaro(benchmark::State& state)
{
    std::vector<std::string> seq1;
    std::vector<std::string> seq2;
    for (int i = 0; i < 256; i++)
        seq1.push_back(generate(MaxLen1));
    for (int i = 0; i < 10000; i++)
        seq2.push_back(generate(MaxLen2));

    size_t num = 0;
    for (auto _ : state) {
        for (size_t j = 0; j < seq2.size(); ++j)
            for (size_t i = 0; i < seq1.size(); ++i)
                benchmark::DoNotOptimize(rapidfuzz::jaro_similarity(seq1[i], seq2[j]));

        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

template <size_t MaxLen1, size_t MaxLen2>
static void BM_Jaro_Cached(benchmark::State& state)
{
    std::vector<std::string> seq1;
    std::vector<std::string> seq2;
    for (int i = 0; i < 256; i++)
        seq1.push_back(generate(MaxLen1));
    for (int i = 0; i < 10000; i++)
        seq2.push_back(generate(MaxLen2));

    size_t num = 0;
    for (auto _ : state) {
        for (const auto& str1 : seq1) {
            rapidfuzz::CachedJaro<char> scorer(str1);
            for (size_t j = 0; j < seq2.size(); ++j)
                benchmark::DoNotOptimize(scorer.similarity(seq2[j]));
        }
        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK_TEMPLATE(BM_Jaro, 8, 8);
BENCHMARK_TEMPLATE(BM_Jaro, 16, 16);
BENCHMARK_TEMPLATE(BM_Jaro, 32, 32);
BENCHMARK_TEMPLATE(BM_Jaro, 64, 64);

BENCHMARK_TEMPLATE(BM_Jaro_Cached, 8, 8);
BENCHMARK_TEMPLATE(BM_Jaro_Cached, 16, 16);
BENCHMARK_TEMPLATE(BM_Jaro_Cached, 32, 32);
BENCHMARK_TEMPLATE(BM_Jaro_Cached, 64, 64);

#ifdef RAPIDFUZZ_SIMD
BENCHMARK_TEMPLATE(BM_Jaro_SIMD, 8, 8);
BENCHMARK_TEMPLATE(BM_Jaro_SIMD, 16, 16);
BENCHMARK_TEMPLATE(BM_Jaro_SIMD, 32, 32);
BENCHMARK_TEMPLATE(BM_Jaro_SIMD, 64, 64);
#endif

BENCHMARK_TEMPLATE(BM_Jaro, 8, 1000);
BENCHMARK_TEMPLATE(BM_Jaro, 16, 1000);
BENCHMARK_TEMPLATE(BM_Jaro, 32, 1000);
BENCHMARK_TEMPLATE(BM_Jaro, 64, 1000);

BENCHMARK_TEMPLATE(BM_Jaro_Cached, 8, 1000);
BENCHMARK_TEMPLATE(BM_Jaro_Cached, 16, 1000);
BENCHMARK_TEMPLATE(BM_Jaro_Cached, 32, 1000);
BENCHMARK_TEMPLATE(BM_Jaro_Cached, 64, 1000);

#ifdef RAPIDFUZZ_SIMD
BENCHMARK_TEMPLATE(BM_Jaro_SIMD, 8, 1000);
BENCHMARK_TEMPLATE(BM_Jaro_SIMD, 16, 1000);
BENCHMARK_TEMPLATE(BM_Jaro_SIMD, 32, 1000);
BENCHMARK_TEMPLATE(BM_Jaro_SIMD, 64, 1000);
#endif

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