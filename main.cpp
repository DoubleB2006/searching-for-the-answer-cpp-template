#include <iostream>

#include "src/words.hpp"

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input.txt> <output.json>\n";
        cerr << "Example: ./inverted_index data.txt index.json\n";
        return 1;
    }

    const string input_path = argv[1];
    const string output_path = argv[2];

    try {
        auto t0 = chrono::steady_clock::now();
        string text = read_file(input_path);
        auto t1 = chrono::steady_clock::now();

        vector<string> tokens = tokenize_preserve_hyphen(text);
        auto t2 = chrono::steady_clock::now();

        auto index = build_index(tokens);
        auto t3 = chrono::steady_clock::now();

        write_index_json(index, output_path);
        auto t4 = chrono::steady_clock::now();

        // Benchmark summary
        auto read_ms   = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();
        auto token_ms  = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
        auto build_ms  = chrono::duration_cast<chrono::milliseconds>(t3 - t2).count();
        auto write_ms  = chrono::duration_cast<chrono::milliseconds>(t4 - t3).count();
        auto total_ms  = chrono::duration_cast<chrono::milliseconds>(t4 - t0).count();

        cerr << "Read:    " << read_ms  << " ms\n";
        cerr << "Tokenize:" << token_ms << " ms\n";
        cerr << "Build:   " << build_ms << " ms\n";
        cerr << "Write:   " << write_ms << " ms\n";
        cerr << "Total:   " << total_ms << " ms\n";
        cerr << "Tokens (all): " << tokens.size() << ", Unique terms: " << index.size() << "\n";

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}

