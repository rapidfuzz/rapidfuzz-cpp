#include <benchmark/benchmark.h>
#include <random>
#include <rapidfuzz/details/intrinsics.hpp>
#include <rapidfuzz/distance/Levenshtein.hpp>
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

using bv_bits = uint64_t;
static const uint64_t width = 64;

static const uint64_t masks[64] = {
  //0x0000000000000000,
  0x0000000000000001ull,0x0000000000000003ull,0x0000000000000007ull,0x000000000000000full,
  0x000000000000001full,0x000000000000003full,0x000000000000007full,0x00000000000000ffull,
  0x00000000000001ffull,0x00000000000003ffull,0x00000000000007ffull,0x0000000000000fffull,
  0x0000000000001fffull,0x0000000000003fffull,0x0000000000007fffull,0x000000000000ffffull,
  0x000000000001ffffull,0x000000000003ffffull,0x000000000007ffffull,0x00000000000fffffull,
  0x00000000001fffffull,0x00000000003fffffull,0x00000000007fffffull,0x0000000000ffffffull,
  0x0000000001ffffffull,0x0000000003ffffffull,0x0000000007ffffffull,0x000000000fffffffull,
  0x000000001fffffffull,0x000000003fffffffull,0x000000007fffffffull,0x00000000ffffffffull,
  0x00000001ffffffffull,0x00000003ffffffffull,0x00000007ffffffffull,0x0000000fffffffffull,
  0x0000001fffffffffull,0x0000003fffffffffull,0x0000007fffffffffull,0x000000ffffffffffull,
  0x000001ffffffffffull,0x000003ffffffffffull,0x000007ffffffffffull,0x00000fffffffffffull,
  0x00001fffffffffffull,0x00003fffffffffffull,0x00007fffffffffffull,0x0000ffffffffffffull,
  0x0001ffffffffffffull,0x0003ffffffffffffull,0x0007ffffffffffffull,0x000fffffffffffffull,
  0x001fffffffffffffull,0x003fffffffffffffull,0x007fffffffffffffull,0x00ffffffffffffffull,
  0x01ffffffffffffffull,0x03ffffffffffffffull,0x07ffffffffffffffull,0x0fffffffffffffffull,
  0x1fffffffffffffffull,0x3fffffffffffffffull,0x7fffffffffffffffull,0xffffffffffffffffull,
};


int dist_bytes (const char * a, int alen, const char * b,  int blen) {

    int amin = 0;
    int amax = alen-1;
    int bmin = 0;
    int bmax = blen-1;

//if (1) {
    while (amin <= amax && bmin <= bmax && a[amin] == b[bmin]) {
        amin++;
        bmin++;
    }
    while (amin <= amax && bmin <= bmax && a[amax] == b[bmax]) {
        amax--;
        bmax--;
    }
//}

    // if one of the sequences is a complete subset of the other,
    // return difference of lengths.
    if ((amax < amin) || (bmax < bmin)) { return abs(alen - blen); }

    int m = amax-amin + 1;

    if ((amax - amin) < width) {

        // for codepoints in the low range we use fast table lookup
        //int low_chars = 256;
        //static bv_bits posbits[256] = { 0 };
        bv_bits posbits[128] = { 0 };
        int i;

        //for (i=0; i < 128; i++) { posbits[i] = 0; }

        for (i=0; i < m; i++) {
            posbits[(unsigned int)a[i+amin]] |= 0x1ull << i;
        }

        int diff = m;
        bv_bits mask = 0x1ull << (m - 1);
        bv_bits VP   = masks[m - 1];
        bv_bits VN   = 0;

        bv_bits y;
        for (i=bmin; i <= bmax; i++) {
            y = posbits[(unsigned int)b[i]];

            bv_bits X  = y | VN;
            bv_bits D0 = ((VP + (X & VP)) ^ VP) | X;
            bv_bits HN = VP & D0;
            bv_bits HP = VN | ~(VP|D0);
            X  = (HP << 1) | 0x1ull;
            VN = X & D0;
            VP = (HN << 1) | ~(X | D0);
            if (HP & mask) { diff++; }
            if (HN & mask) { diff--; }
        }
        return diff;
    }
    else {

        int diff = m;

        int kmax = (m) / width;
        if ( m % width ) { kmax++; }

        // for codepoints in the low range we use fast table lookup
        //int low_chars = 256;
        bv_bits *posbits = (bv_bits *) alloca ( 256 * kmax *  sizeof(bv_bits) );

        //bv_bits *posbits[256 * kmax] = { 0 };

        int i;
        int k;

        for (i=0; i < 128; i++) {
            for (k=0; k < kmax; k++) {
                posbits[i * kmax + k] = 0;
            }
        }

        for (i=0; i < m; i++) {
            posbits[ (unsigned int)a[i+amin] * kmax + (unsigned int)(i/width) ]
                |= 0x1ull << (i % width);
        }

        bv_bits mask[kmax];
        for (k=0; k < kmax; k++) { mask[k] = 0; }
        mask[kmax-1] = 0x1ull << ((m-1) % width);

        // bv_bits VP   = masks[m - 1];
        bv_bits VPs[kmax];
        for (k=0; k < kmax; k++) { VPs[k] = 0xffffffffffffffffull; }
        VPs[kmax-1] =  masks[ (unsigned int)((m-1) % width) ];

        bv_bits VNs[kmax];
        for (k=0; k < kmax; k++) { VNs[k] = 0; }

        bv_bits y, X, D0, HN, HP;

        bv_bits HNcarry;
        bv_bits HPcarry;

        for (i=bmin; i <= bmax; i++) {

            HNcarry = 0;
            HPcarry = 1;
            for (int k=0; k < kmax; k++ ) {
                y = posbits[(unsigned int)b[i] * kmax + k];

                X  = y | HNcarry | VNs[k];
                D0 = ((VPs[k] + (X & VPs[k])) ^ VPs[k]) | X;
                HN = VPs[k] & D0;
                HP = VNs[k] | ~(VPs[k] | D0);
                X  = (HP << 1) | HPcarry;
                HPcarry = HP >> (width-1) & 0x1ull;
                VNs[k]  = (X & D0);
                VPs[k]  = (HN << 1) | (HNcarry) | ~(X | D0);
                HNcarry = HN >> (width-1) & 0x1ull;

                if      (HP & mask[k]) { diff++; }
                else if (HN & mask[k]) { diff--; }
            }
        }
        return diff;
    }
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