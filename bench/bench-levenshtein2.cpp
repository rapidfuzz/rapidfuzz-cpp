#include <benchmark/benchmark.h>
#include <random>
#include <rapidfuzz/details/intrinsics.hpp>
#include <rapidfuzz/distance/Levenshtein.hpp>
#include <string>
#include <vector>
#include "levbv.h"

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

std::basic_string<uint32_t> generate_unicode(int max_length)
{
    std::string possible_characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(possible_characters.size() - 1));
    std::basic_string<uint32_t> ret;
    for (int i = 0; i < max_length; i++) {
        int random_index = dist(engine);
        ret += static_cast<uint32_t>(possible_characters[static_cast<size_t>(random_index)] + 1000);
    }
    return ret;
}

template <size_t MaxLen>
static void BM_Levenshtein2(benchmark::State& state)
{
    std::vector<std::string> seq1;
    std::vector<std::string> seq2;
    for (int i = 0; i < 256; i++)
        seq1.push_back(generate(MaxLen));
    for (int i = 0; i < 10000; i++)
        seq2.push_back(generate(MaxLen));

    size_t num = 0;
    for (auto _ : state) {
        for (size_t j = 0; j < seq2.size(); ++j)
            for (size_t i = 0; i < seq1.size(); ++i)
                benchmark::DoNotOptimize(dist_bytes(seq1[i].data(), static_cast<uint32_t>(seq1[i].size()), seq2[j].data(), static_cast<uint32_t>(seq2[j].size())));

        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

template <size_t MaxLen>
static void BM_Levenshtein(benchmark::State& state)
{
    std::vector<std::string> seq1;
    std::vector<std::string> seq2;
    for (int i = 0; i < 256; i++)
        seq1.push_back(generate(MaxLen));
    for (int i = 0; i < 10000; i++)
        seq2.push_back(generate(MaxLen));

    size_t num = 0;
    for (auto _ : state) {
        for (size_t j = 0; j < seq2.size(); ++j)
            for (size_t i = 0; i < seq1.size(); ++i)
                benchmark::DoNotOptimize(rapidfuzz::levenshtein_distance(seq1[i], seq2[j]));

        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

template <size_t MaxLen>
static void BM_Unicode_Levenshtein2(benchmark::State& state)
{
    std::vector<std::basic_string<uint32_t>> seq1;
    std::vector<std::basic_string<uint32_t>> seq2;
    for (int i = 0; i < 256; i++)
        seq1.push_back(generate_unicode(MaxLen));
    for (int i = 0; i < 10000; i++)
        seq2.push_back(generate_unicode(MaxLen));

    size_t num = 0;
    for (auto _ : state) {
        for (size_t j = 0; j < seq2.size(); ++j)
            for (size_t i = 0; i < seq1.size(); ++i)
                benchmark::DoNotOptimize(dist_hybrid(seq1[i].data(), static_cast<uint32_t>(seq1[i].size()), seq2[j].data(), static_cast<uint32_t>(seq2[j].size())));

        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

template <size_t MaxLen>
static void BM_Unicode_Levenshtein(benchmark::State& state)
{
    std::vector<std::basic_string<uint32_t>> seq1;
    std::vector<std::basic_string<uint32_t>> seq2;
    for (int i = 0; i < 256; i++)
        seq1.push_back(generate_unicode(MaxLen));
    for (int i = 0; i < 10000; i++)
        seq2.push_back(generate_unicode(MaxLen));

    size_t num = 0;
    for (auto _ : state) {
        for (size_t j = 0; j < seq2.size(); ++j)
            for (size_t i = 0; i < seq1.size(); ++i)
                benchmark::DoNotOptimize(rapidfuzz::levenshtein_distance(seq1[i], seq2[j]));

        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

template <size_t MaxLen>
static void BM_Levenshtein_Cached(benchmark::State& state)
{
    std::vector<std::string> seq1;
    std::vector<std::string> seq2;
    for (int i = 0; i < 256; i++)
        seq1.push_back(generate(MaxLen));
    for (int i = 0; i < 10000; i++)
        seq2.push_back(generate(MaxLen));

    size_t num = 0;
    for (auto _ : state) {
        for (const auto& str1 : seq1) {
            rapidfuzz::CachedLevenshtein<char> scorer(str1);
            for (size_t j = 0; j < seq2.size(); ++j)
                benchmark::DoNotOptimize(scorer.similarity(seq2[j]));
        }
        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein2, 8);
BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein2, 16);
BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein2, 32);
BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein2, 64);
BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein2, 200);

BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein, 8);
BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein, 16);
BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein, 32);
BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein, 64);
BENCHMARK_TEMPLATE(BM_Unicode_Levenshtein, 200);


BENCHMARK_TEMPLATE(BM_Levenshtein2, 8);
BENCHMARK_TEMPLATE(BM_Levenshtein2, 16);
BENCHMARK_TEMPLATE(BM_Levenshtein2, 32);
BENCHMARK_TEMPLATE(BM_Levenshtein2, 64);

BENCHMARK_TEMPLATE(BM_Levenshtein, 8);
BENCHMARK_TEMPLATE(BM_Levenshtein, 16);
BENCHMARK_TEMPLATE(BM_Levenshtein, 32);
BENCHMARK_TEMPLATE(BM_Levenshtein, 64);

BENCHMARK_TEMPLATE(BM_Levenshtein_Cached, 8);
BENCHMARK_TEMPLATE(BM_Levenshtein_Cached, 16);
BENCHMARK_TEMPLATE(BM_Levenshtein_Cached, 32);
BENCHMARK_TEMPLATE(BM_Levenshtein_Cached, 64);


BENCHMARK_MAIN();