#include "StudentSpellCheck.h"
#include <string>
#include <vector>

#include <cctype>
#include <fstream>

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

const int VALID_CHARS = 27;

StudentSpellCheck::StudentSpellCheck() {
	m_head = nullptr; // initialize to nullptr, change when you load
}

StudentSpellCheck::~StudentSpellCheck() {
	// Clear dynamically allocated nodes
	freeTrie(m_head);
	// TODO
}

void StudentSpellCheck::freeTrie(Node* curr) {
	if (curr == nullptr) return;
	for (int i = 0; i < VALID_CHARS; ++i) {
		freeTrie(curr->next[i]);
	}
	delete curr;
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	// Read in file
	std::ifstream infile(dictionaryFile);
	if (!infile) return false;
	// Unload current dictionary
	freeTrie(m_head);
	m_head = new Node;
	std::string s;
	// Add words to trie
	while (getline(infile, s)) {
		addWord(s);
	}
	return true; // TODO
}

void StudentSpellCheck::addWord(std::string word) {
	Node* curr = m_head;
	// add characters of word to trie
	for (int i = 0; i < word.length(); ++i) {
		// Add if valid character (alpha/apostrophe)
		if (isValid(word[i])) {
			char ch = tolower(word[i]);
			// check if character is in node's children
			int pos = charToPos(ch);
			Node* next = curr->next[pos];
			// create next if does not exist
			if (next == nullptr) {
				next = new Node;
				curr->next[pos] = next;
			}
			curr = next;
		}
	}
	// curr is at final character, indicate it is a word
	curr->isWord = true;
}

int StudentSpellCheck::charToPos(char ch) const {
	return ch == '\'' ? 0 : tolower(ch) - 'a';
}

char StudentSpellCheck::posToChar(int pos) const {
	return pos == 0 ? '\'' : 'a' + pos;
}

bool StudentSpellCheck::isValid(char ch) const {
	return isalpha(ch) || ch == '\'';
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	// Check if word is in trie (assumes word consists of only valid characters)
	if (isSubword(0, m_head, word)) return true;

	// Otherwise, check dictionary entries
	suggestions.clear();
	// Convert word to lower case
	int len = word.length();
	for (int i = 0; i < len; ++i) {
		word[i] = tolower(word[i]);
	}
	// Partition by which character differs
	Node* curr = m_head;
	for (int i = 0; i < len; ++i) {
		findSuggestions(i, curr, word, max_suggestions, suggestions); // base, word, max_suggestions, suggestion
		if (suggestions.size() >= max_suggestions) break; // reached the max
		curr = curr->next[charToPos(word[i])];
		if (curr == nullptr) break; // no valid suggestions past this point
	}

	return false; // TODO
}

// Check if subword exists in trie
bool StudentSpellCheck::isSubword(int start, Node* base, const std::string& word) const  {
	if (base == nullptr) return false;
	Node* curr = base;
	int len = word.length();
	for (int i = start; i < len; ++i) {
		curr = curr->next[charToPos(word[i])];
		if (curr == nullptr) break;
	}
	return curr != nullptr && curr->isWord;
}

// Finds words that differ only by the character at position start
void StudentSpellCheck::findSuggestions(int start, Node* base, std::string word, int max_suggestions, std::vector<std::string>& suggestions) const {
	int len = word.length();
	for (int i = 0; i < VALID_CHARS; ++i) {
		if (suggestions.size() >= max_suggestions) return; // prevent from going over max
		if (isSubword(start + 1, base->next[i], word)) {
			word[start] = posToChar(i);
			suggestions.push_back(word);
		}
	}
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	int start = 0;
	int len = line.length();
	problems.clear();
	// Note character at end should be a null character - check end to make sure words at end are parsed
	for (int i = 1; i <= len; ++i) {
		// If curr valid and prev invalid, start of a new word
		if (isValid(line[i]) && !isValid(line[i - 1])) start = i;
		// If curr invalid and prev valid, end of a word -- check validity of word
		else if (!isValid(line[i]) && isValid(line[i - 1])) {
			// Add if problem word
			if (!isSubword(0, m_head, line.substr(start, i - start))) {
				SpellCheck::Position pos;
				pos.start = start;
				pos.end = i - 1;
				problems.push_back(pos);
			}
		}
	}
	// TODO
}
