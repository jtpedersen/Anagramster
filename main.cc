// -*- compile-command: CXXFLAGS="-std=c++1y -Wall -O3" make -k main && ./main | sort -u; -*-
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
	    children[front(str) - 'a'] = child;
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
	    root->insert(line);
	    lines++;
	}
    }
    
    bool isWord;
    array<trie_ptr, 26> children;
    trie_ptr operator[] (size_t idx) const {
	assert(idx - 'a' >= 0 || ! printf("%d -> %d ( %c )\n", idx, idx - 'a', idx));
	assert(idx - 'a' < 26 || ! printf("%d -> %d ( %c )\n", idx, idx - 'a', idx));
	return children[idx - 'a']; };

  trie_ptr operator[] (char c) const {
    return (*this)[static_cast<size_t>(c)];
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

int main(int argc, char **) {

    chrono::time_point<chrono::system_clock> start, lap, end;
    start = chrono::system_clock::now();

    Trie::loadDictonary("words.txt");

    lap = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = lap-start;
    cout << "finished reading dictionay  in " << elapsed_seconds.count() << "s" << endl;
    
    auto letters = multiset<char>();
    for (auto c : "anette heick") {
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

