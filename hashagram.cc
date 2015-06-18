// -*- compile-command: CXXFLAGS="-std=c++1y -Wall -O3" make -k hashagram && ./hashagram  -*-

/**
 * uses a hashing approach to fin anagrams
 */

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <cassert>
#include <chrono>
#include <fstream>
#include <sstream>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <functional>

using namespace std;

/// key = lexicografically lowercasedsorted string 
string createKey(const string& word) {
  string key(word);
  transform(key.begin(), key.end(), key.begin(), ::tolower);
  sort(key.begin(), key.end());
  return key;
}

unordered_multimap<string, string> dictionary;
void loadDictonary(string filename) {
  chrono::time_point<chrono::system_clock> start;
  start = chrono::system_clock::now();
  ifstream ifs(filename);
  std::string line;
  size_t lines = 0;
  while( getline(ifs, line) ) {
    transform(line.begin(), line.end(), line.begin(), ::tolower);
    dictionary.emplace(make_pair(createKey(line), line));
    lines++;
  }
  chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - start;
  // cout << "finished reading " << lines << " words into dictionary in " << elapsed_seconds.count() << "s" << endl;
}


set<string> permutations(set<string> xs, set<string> ys) {
  set<string> res;
  for(auto x: xs) {
    for(auto y: ys) {
      res.emplace(x + " " + y);
    }
  }
  return res;
}

unordered_map<string, set<string>> memotable;

set<string> findAnagrams(string word) {
  auto key = createKey(word);
  if (memotable.find(key) != memotable.end())
    return memotable[key];
  // cout << "findAnagrams for : " << word << endl;
  set<string> results;
  auto range = dictionary.equal_range(key);
  for (auto it = range.first; it != range.second; ++it) {
      results.emplace(it->second);
  }
  
  // recurse!
  for(unsigned int i = 1; i < word.size(); i++) {
    auto prefixes = findAnagrams(word.substr(0,i));
    auto suffixes = findAnagrams(word.substr(i));
    if ( ! (prefixes.empty() && suffixes.empty()) ) {
      auto perms = permutations(prefixes, suffixes);
      for(auto p : perms)
	results.emplace(p);
    }
  }
  memotable.emplace(make_pair(key, results));
  return results;
}

int main(int argc, char *argv[]) {

  loadDictonary("out.txt");
  string input("");
  if (argc > 1) {
    for(int i = 1; i < argc; i++) {
      input += argv[i];
    }
  } else {
    input = "find bare de anagrammer";
  }
  
  input.erase(remove_if(input.begin(), input.end(), [](char c) {return !isalpha(c);}), input.end());
  // cout << "Looking for anagrams with: " << input << endl;
  auto results = findAnagrams(input);
  for (auto s : results)
    cout << s << endl;
  return 0;
}

