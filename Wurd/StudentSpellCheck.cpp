#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>


SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

//construct the head pointer to nullptr
StudentSpellCheck::StudentSpellCheck() {
	head = nullptr;
}

//construct the trie tree by setting each child to nullptr
//and set bool to false
StudentSpellCheck::Trie::Trie() {
	isTheEnd = false;

	for (int i = 0; i < 27; i++)
		children[i] = nullptr;
}

//insert the word into the trie tree
void StudentSpellCheck::insert(Trie* root, std::string word) {
	struct Trie* pCrawl = root;
	//traverse through the word
	for (int i = 0; i < word.length(); i++)
	{
		int index;
		//if the char is ' , then set its position to 26
		if (word[i] == '\'') {
			index = 26;
		}
		else {
			index = word[i] - 'a';
		}
		//insert the char is didnt find
		if (!pCrawl->children[index])
			pCrawl->children[index] = new Trie();

		pCrawl = pCrawl->children[index];
	}

	// mark last node as leaf 
	pCrawl->isTheEnd = true;

}

//search the word in the trie tree
bool StudentSpellCheck::search(Trie* root, std::string word) {
	struct Trie* pCrawl = root;

	//traverse through the word
	for (int i = 0; i < word.length(); i++)
	{
		int index;
		//set the char to the appropriated index
		if (word[i] == '\'') {
			index = 26;
		}
		else {
			index = word[i] - 'a';
		}
		//search for each char
		if (!pCrawl->children[index])
			return false;

		pCrawl = pCrawl->children[index];
	}
	//check if the word is valid
	return (pCrawl != NULL && pCrawl->isTheEnd);
}

//delete all chars in trie
void StudentSpellCheck::deleteAll(Trie* root) {
	if (root == nullptr)
		return;

	//traverse the trie using postorder recursion
	for (int i = 0; i < 27; i++) {
		deleteAll(root->children[i]);
	}
	//delete the char
	delete root;
	root = nullptr;
}

//destruct the student spell check by delete all the
//chars in the trie
StudentSpellCheck::~StudentSpellCheck() {
	Trie* temp = head;
	deleteAll(temp);
	head = nullptr;
}

//This function must run in O(N) time where N is the number of lines in the dictionary file,
//assuming that there's a constant upper bound on the length of an input line.
bool StudentSpellCheck::load(std::string dictionaryFile) {
 //loading a new dictionary, clear previous dictionary

	std::ifstream infile(dictionaryFile);    // infile is a name of our choosing
	if (!infile)		        // Did opening the file fail?
	{
		return false;
	}
	deleteAll(head);
	std::string s;
	head = new Trie;
	Trie* temp = head;
	// getline returns infile; the while tests its success/failure state
	while (getline(infile, s))
	{
		//insert each word from the dictionary into the trie
		insert(temp, s);
	}
	return true;
}

//This function must run in O(L2 + maxSuggestions) time where L is the length of the word
//being searched for
bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	for (int i = 0; i < word.size(); i++) {
		word[i] = tolower(word[i]);
	}
	Trie* temp = head;
	bool found = search(temp, word);
	if (found == true)
		return true;
	//clear what's inside suggetions
	suggestions.clear();
	//create a array for all characters to replace the word
	char allChar[27] = {'a','b','c','d','e','f','g',
		                'h','i','j','k','l','m','n',
		                'o','p','q','r','s','t',
		                'u','v','w','x','y','z','\''};
	std::vector<std::string> fakeSuggestion;
	//create a vector to store word that is different from one
	
	//search for each char of word
	for (int i = 0; i < word.size(); i++) {
		std::string tempWord = word;
		//search for every char of word with 27 different alphabets
		for (int j = 0; j < 27; j++) {
			tempWord[i] = allChar[j];
			Trie* temp = head;
			//if this word which is one letter differ from the original word
			//push to the vector
			if (search(temp, tempWord)) {
				fakeSuggestion.push_back(tempWord);
			}
		}
	
	}
	//push back the max number of suggestions into the vector
	std::vector<std::string>::iterator it = fakeSuggestion.begin();
	for (int i = 0; i < max_suggestions; i++) {
		if (it != fakeSuggestion.end()) {
			suggestions.push_back(*it);
			it++;
		}
	}
	
	return false;
}

//This function must run in O(S+W*L) time where S is the length of the line passed in, W is the
//number of words in the line, and L is the maximum length of a checked word.
void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	//create a array for all characters that are valid to make a word
	char allChar[27] = { 'a','b','c','d','e','f','g',
						'h','i','j','k','l','m','n',
						'o','p','q','r','s','t',
						'u','v','w','x','y','z','\'' };
	int start = 0;
	int end = 0;
	std::string word = "";
	bool prev = false;
	std::string FakeLine = "";
	for (int i = 0; i < line.size(); i++) {
		char k =  tolower(line[i]);
		FakeLine = FakeLine + k;
	}
	//traverse through the line
	for (int i = 0; i < line.size(); i++) {
		//check if the chracter is a valid letter to form a word
		bool isChar = false;
		for (int j = 0; j < 27; j++) {
			if (FakeLine[i] == allChar[j]) {
				isChar = true;
				break;
			}
		}
		//check if this char is a valid char and the previous char is invalid
		if (isChar == true && prev == false) {
			start = i;
			end = start;
			word = word + FakeLine[i];
		}
		//if a char is a valid char and it is in the middle of the words
		else if (isChar == true && prev == true) {
			end++;
			word = word + FakeLine[i];
		}
		//if this is the end of the word
		else if (isChar == false && prev == true) {
			Trie* temp = head;
			bool isWord = search(temp, word);
			// if this is a invalid word
			if (isWord == false) {
				SpellCheck::Position pos;
				pos.start = start;
				pos.end = end;
				problems.push_back(pos);
			}
			word = "";
		}
		//store the information of the previous char
		prev = isChar;
	}
	//if pass the for loop, but the last char is valid
	if (word!= "") {
		Trie* temp = head;
		bool isWord = search(temp, word);
		// if this is a invalid word
		if (isWord == false) {
			SpellCheck::Position pos;
			pos.start = start;
			pos.end = end;
			problems.push_back(pos);
		}
	}
}
