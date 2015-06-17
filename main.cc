// -*- compile-command: CXXFLAGS="-std=c++1y -Wall -O3" make -k main && ./main  -*-
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

using namespace std;
class Trie;
using trie_ptr = shared_ptr<Trie>;
//to see the difference between const string& const char* etc
// it seems like const string is fastest? +- harddisk speed
using strtype = const string;

class Trie {
public:
    Trie() : isWord(false) {};
    char front(strtype str) const { return tolower(str[0]); };
    bool empty(strtype str) const { return str[0] == '\0';};
    bool hasChild(const char c) const { return nullptr != (*this)[c];}
    trie_ptr successor(strtype str) const { return (*this)[front(str)]; }
    void insert(strtype str) {
	if ( empty(str) ) {
	    isWord = true;
	    return;
	}
	auto child = successor(str);
	if (nullptr == child) {
	    child = make_shared<Trie>();
	    children[front(str)] = child;
	}
	child->insert(str.substr(1));
    }

    bool find(strtype str) const {
	if ( empty(str) )
	    return isWord;
	auto child = successor(str);
	if (nullptr == child)
	    return false;
	return child->find(str.substr(1));
    }
    
    void print(string prefix) {
	if (isWord)
	    cout << prefix << endl;
	for(char c = 'a'; c <= 'z'; c++) {
	    if (hasChild(c))
		(*this)[c]->print(prefix + c);
	    // else
	    // 	cout << "c is :" << c << endl;
	    // 	child->print(prefix + c);
	}
    }

    static void print() {
	Trie::root->print("");
    }

    static void loadDictonary(string filename) {
	auto root = make_shared<Trie>();
	Trie::root = root;
	ifstream ifs(filename);
	std::string line;
	size_t lines = 0;
	while( getline(ifs, line) ) {
	  transform(line.begin(), line.end(), line.begin(), ::tolower);
	    root->insert(line);
	    lines++;
	}
    }
    
    bool isWord;
  unordered_map<char, trie_ptr> children;
  trie_ptr operator[] (char c) const {
    auto child = children.find(c);
    if ( child == children.end() )
      return nullptr;
    return child->second;
  }
    static trie_ptr root;
};
trie_ptr Trie::root = nullptr;

void lookup(const Trie& root, string word) {
    cout << word << (root.find(word.c_str()) ? " was " : " was not " ) << "in the Trie" <<endl;
}

set<string> results;

void search(trie_ptr t, const multiset<char> letters, std::string res) {
  if (0 == letters.size()) {
    if (t->isWord)
      results.emplace(res);
    if (0 == (results.size() % 100) ){
      cout << ".";
      flush(cout);
    }
    return;
  }

  for (auto letter: letters) {
    auto tp = (*t)[letter];
    if (tp) {
      auto tmp = multiset<char>(letters);
      tmp.erase(tmp.find(letter));
      search(tp, tmp, res + letter);
    }
  }
  
  if (t->isWord) {
    search(Trie::root, letters, res + " ");
  }
}

int main(int argc, char *argv[]) {

    Trie::loadDictonary("out.txt");
    string input("");
    if (argc > 1) {
      for(int i = 1; i < argc; i++) {
	input += argv[i];
      }
    } else {
      input = "Jacob Pedersen";
    }
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    
    chrono::time_point<chrono::system_clock> start, lap, end;
    start = chrono::system_clock::now();


    lap = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = lap-start;
    cout << "finished reading dictionary in " << elapsed_seconds.count() << "s" << endl;
    
    auto letters = multiset<char>();
    for (auto c : input) {
      if (isalpha(c))
	letters.emplace(c);
    }
    search(Trie::root, letters, "");
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-lap;
    cout << "found all " << results.size() << " solutions in " << elapsed_seconds.count() << "s" << endl;
    for (auto s : results)
      cout << s << endl;
    return 0;
}

