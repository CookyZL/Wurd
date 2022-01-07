#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
	StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	// Trie node
	struct Trie
	{
		Trie();
		struct Trie* children[27];
		bool isTheEnd;
	};

	void insert(Trie* root,std::string word);
	bool search(Trie* root,std::string word);
	void deleteAll(Trie* root);
	Trie* head;
};

#endif  // STUDENTSPELLCHECK_H_
