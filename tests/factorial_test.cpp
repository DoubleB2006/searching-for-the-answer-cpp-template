#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "../src/words.hpp"   // declares: tokenize_preserve_hyphen, build_index

// ---------- tiny helpers ----------
template <class Map>
static bool has_key(const Map& m, const typename Map::key_type& k) {
    return m.find(k) != m.end();
}
// ----------------------------------

TEST_CASE("tokenize: lowercase + keep hyphenated words") {
    std::string s = "Apple, APPLE; well-known -- high-quality! Don't";
    auto toks = tokenize_preserve_hyphen(s);

    REQUIRE(std::find(toks.begin(), toks.end(), "apple") != toks.end());
    REQUIRE(std::find(toks.begin(), toks.end(), "well-known") != toks.end());
    REQUIRE(std::find(toks.begin(), toks.end(), "high-quality") != toks.end());

    // punctuation shouldn't become tokens
    REQUIRE(std::find(toks.begin(), toks.end(), ",")  == toks.end());
    REQUIRE(std::find(toks.begin(), toks.end(), "--") == toks.end());
}

TEST_CASE("build_index: simple sentence positions (0-based)") {
    std::string text = "the cat and the dog";
    auto toks = tokenize_preserve_hyphen(text);
    auto idx  = build_index(toks);

    // assuming stop words remove 'the' and 'and'
    REQUIRE_FALSE(has_key(idx, "the"));
    REQUIRE_FALSE(has_key(idx, "and"));

    // positions: cat at 1, dog at 4 before stop-word removal
    REQUIRE(has_key(idx, "cat"));
    REQUIRE(idx["cat"] == std::vector<size_t>{1});

    REQUIRE(has_key(idx, "dog"));
    REQUIRE(idx["dog"] == std::vector<size_t>{4});
}

TEST_CASE("build_index: repeats give increasing positions") {
    std::string text = "alpha beta alpha beta alpha";
    auto toks = tokenize_preserve_hyphen(text);
    auto idx  = build_index(toks);

    if (has_key(idx, "alpha")) REQUIRE(idx["alpha"] == std::vector<size_t>{0,2,4});
    if (has_key(idx, "beta"))  REQUIRE(idx["beta"]  == std::vector<size_t>{1,3});
}

// -------- BENCHMARKS (simple, practical) --------
TEST_CASE("bench: tokenize and build_index on larger input") {
    const std::string seed =
        "The brown cat and the blue rat live in the brown house. "
        "They enjoy live music concerts. ";

    std::string big;
    big.reserve(seed.size() * 2000);
    for (int i = 0; i < 2000; ++i) big += seed;

    BENCHMARK("tokenize_preserve_hyphen( ~100k chars )") {
        return tokenize_preserve_hyphen(big).size();
    };

    BENCHMARK("build_index( tokens from ~100k chars )") {
        auto toks = tokenize_preserve_hyphen(big);
        auto idx  = build_index(toks);
        return idx.size(); // unique terms
    };
}
