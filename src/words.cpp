#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cctype>
#include <chrono>
using namespace std;

static const unordered_set<string> STOP_WORDS = {
    "a","an","and","the","to","for","of","in","on","at","is","are","was","were",
    "be","been","being","he","she","it","they","you","your","yours","we","us","our",
    "i","me","my","mine","this","that","these","those","with","as","by","from","or",
    "but","if","then","so","because","about","into","over","under","up","down","out"
};

string read_file(const string& path) {
    ifstream fin(path, ios::binary);
    if (!fin) throw runtime_error("Could not open input file: " + path);
    stringstream ss; ss << fin.rdbuf();
    return ss.str();
}

// Helper: check if character is part of a word (letters/digits)
bool is_word_char(char c) { return std::isalnum(static_cast<unsigned char>(c)); }

// Tokenize while keeping hyphens that are BETWEEN two word characters.
// Example: "well-known" -> one token, but "--" or trailing/leading hyphens are treated as separators.
vector<string> tokenize_preserve_hyphen(const string& text) {
    vector<string> tokens; 
    string cur;
    auto flush = [&](){ if(!cur.empty()){ tokens.push_back(cur); cur.clear(); } };

    for (size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        unsigned char uch = static_cast<unsigned char>(ch);
        char lo = static_cast<char>(std::tolower(uch));

        if (is_word_char(lo)) {
            cur.push_back(lo);
            continue;
        }

        // hyphen handling
        if (lo=='-') {
            bool left = (i>0) && is_word_char(static_cast<unsigned char>(text[i-1]));
            bool right = (i+1<text.size()) && is_word_char(static_cast<unsigned char>(text[i+1]));
            if (left && right) { cur.push_back('-'); continue; }
        }

        // Apostrophes are removed (e.g., don't -> dont) by delimiting
        // All other punctuation becomes a separator
        flush();
    }
    flush();
    return tokens;
}

// Build inverted index: word -> vector of positions (0-based indices in token stream)
unordered_map<string, vector<size_t>> build_index(const vector<string>& tokens) {
    unordered_map<string, vector<size_t>> idx; idx.reserve(tokens.size());
    for (size_t pos = 0; pos < tokens.size(); ++pos) {
        const string& w = tokens[pos];
        if (STOP_WORDS.count(w)) continue; // skip common words
        idx[w].push_back(pos);
    }
    return idx;
}

// Write JSON-like output matching the lab's example.
void write_index_json(const unordered_map<string, vector<size_t>>& idx, const string& out_path) {
    // Collect and sort keys alphabetically
    vector<string> keys; keys.reserve(idx.size());
    for (auto& kv : idx) keys.push_back(kv.first);
    sort(keys.begin(), keys.end());

    ofstream fout(out_path);
    if (!fout) throw runtime_error("Could not open output file: " + out_path);

    fout << "{\n";
    for (size_t i = 0; i < keys.size(); ++i) {
        const string& k = keys[i];
        fout << "  \"" << k << "\": [";
        const auto& vec = idx.at(k);
        for (size_t j = 0; j < vec.size(); ++j) {
            fout << vec[j];
            if (j + 1 < vec.size()) fout << ", ";
        }
        fout << "]";
        if (i + 1 < keys.size()) fout << ",";
        fout << "\n";
    }
    fout << "}\n";
}

