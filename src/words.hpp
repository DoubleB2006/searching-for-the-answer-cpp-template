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


string read_file(const string& path);
bool is_word_char(char c);
vector<string> tokenize_preserve_hyphen(const string& text);
unordered_map<string, vector<size_t>> build_index(const vector<string>& tokens);
void write_index_json(const unordered_map<string, vector<size_t>>& idx, const string& out_path); 
