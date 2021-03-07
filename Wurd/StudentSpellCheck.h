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
	struct Node {
		Node() : isWord(false) {
			for (int i = 0; i < 27; i++) next[i] = nullptr;
		}
		bool isWord;
		Node* next[27]; // 0 for apostrophe, rest for alphabet
	};
	Node* m_head;
	void addWord(std::string word);
	void freeTrie(Node* curr);
	int charToPos(char ch) const;
	char posToChar(int pos) const;
	bool isValid(char ch) const;
	bool isSubword(int start, Node* base, const std::string& word) const;
	void findSuggestions(int start, Node* base, std::string word, int max_suggestions, std::vector<std::string>& suggestions) const;
};

#endif  // STUDENTSPELLCHECK_H_
