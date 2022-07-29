#include <benchmark/benchmark.h>
#include <random>
#include <rapidfuzz/details/intrinsics.hpp>
#include <rapidfuzz/distance/LCSseq.hpp>
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

int llcs_asci_t (char * a, char * b, uint32_t alen, uint32_t blen) {
    
    uint64_t bits[256];

    uint32_t i;
    for (i=0; i<256; i++) { 
      bits[i] = 0;      
    }

    for (i=0; i < alen; i++){
      	bits[a[i]] |= 1 << (i % 64);
    }    

    uint64_t v = ~0ull;

    for (i=0; i < blen; i++){
      uint64_t p = bits[b[i]];
      uint64_t u = v & p;
      v = (v + u) | (v - u);
    }
    
    //return count_bits(~v);
    return __builtin_popcountll(~v); // portable
    //return _mm_popcnt_u64(~v);
}

template <size_t MaxLen>
static void BM_LCS2(benchmark::State& state)
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
                benchmark::DoNotOptimize(llcs_asci_t(seq1[i].data(), seq2[j].data(), static_cast<uint32_t>(seq1[i].size()), static_cast<uint32_t>(seq2[j].size())));

        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

template <size_t MaxLen>
static void BM_LCS(benchmark::State& state)
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
                benchmark::DoNotOptimize(rapidfuzz::lcs_seq_distance(seq1[i], seq2[j]));

        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

template <size_t MaxLen>
static void BM_LCS_Cached(benchmark::State& state)
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
            rapidfuzz::CachedLCSseq<char> scorer(str1);
            for (size_t j = 0; j < seq2.size(); ++j)
                benchmark::DoNotOptimize(scorer.similarity(seq2[j]));
        }
        num += seq1.size() * seq2.size();
    }

    state.counters["Rate"] = benchmark::Counter(static_cast<double>(num), benchmark::Counter::kIsRate);
    state.counters["InvRate"] = benchmark::Counter(static_cast<double>(num),
                                                   benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK_TEMPLATE(BM_LCS2, 8);
BENCHMARK_TEMPLATE(BM_LCS2, 16);
BENCHMARK_TEMPLATE(BM_LCS2, 32);
BENCHMARK_TEMPLATE(BM_LCS2, 64);

BENCHMARK_TEMPLATE(BM_LCS, 8);
BENCHMARK_TEMPLATE(BM_LCS, 16);
BENCHMARK_TEMPLATE(BM_LCS, 32);
BENCHMARK_TEMPLATE(BM_LCS, 64);

BENCHMARK_TEMPLATE(BM_LCS_Cached, 8);
BENCHMARK_TEMPLATE(BM_LCS_Cached, 16);
BENCHMARK_TEMPLATE(BM_LCS_Cached, 32);
BENCHMARK_TEMPLATE(BM_LCS_Cached, 64);


BENCHMARK_MAIN();