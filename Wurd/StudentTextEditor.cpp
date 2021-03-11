#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>

#include <fstream>
#include <iostream>

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo), m_row(0), m_col(0) 
{
	m_lines.push_back("");
	m_currLine = m_lines.begin(); // TODO
}

StudentTextEditor::~StudentTextEditor()
{
	// TODO
}

bool StudentTextEditor::load(std::string file) {
	std::ifstream infile(file);
	if (!infile) return false;
	m_lines.clear();
	getUndo()->clear();
	std::string s;
	while (getline(infile, s)) {
		if (s.length() > 0 && s.back() == '\r') s.erase(s.end() - 1);
		// Convert tabs to 4 spaces
		for (int i = 0; i < s.length(); i++) {
			if (s[i] == '\t') {
				s.replace(i, 1, 4, ' ');
				i += 3;
			}
		}
		m_lines.push_back(s);
	}
	if (m_lines.empty()) m_lines.push_back(""); // if empty file, push blank line
	m_currLine = m_lines.begin();
	m_row = 0;
	m_col = 0;
	return true;  // TODO
}

bool StudentTextEditor::save(std::string file) {
	std::ofstream outfile(file);
	if (!outfile) return false;
	std::list<std::string>::iterator end = m_lines.end();
	for (std::list<std::string>::iterator it = m_lines.begin(); it != end; ++it) {
		outfile << *it << '\n';
	}

	return true;  // TODO
}

void StudentTextEditor::reset() {
	m_lines.clear();
	m_row = 0;
	m_col = 0;
	// TODO
	m_lines.push_back("");
	m_currLine = m_lines.begin();
	getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
	switch (dir)
	{
	case Dir::UP:
		if (m_row > 0) {
			--m_row;
			--m_currLine;
			if (m_col > m_currLine->length()) m_col = m_currLine->length();
		}
		break;
	case Dir::DOWN:
		if ((m_row + 1) < m_lines.size()) {
			++m_row;
			++m_currLine;
			if (m_col > m_currLine->length()) m_col = m_currLine->length();
		}
		break;
	case Dir::LEFT:
		if (m_col > 0) --m_col;
		else if (m_row > 0) {
			--m_row;
			--m_currLine;
			m_col = m_currLine->size();
		}
		break;
	case Dir::RIGHT:
		if (m_col < m_currLine->size()) ++m_col;
		else if ((m_row + 1) < m_lines.size()) {
			++m_row;
			++m_currLine;
			m_col = 0;
		}
		break;
	case Dir::HOME:
		m_col = 0;
		break;
	case Dir::END:
		m_col = m_currLine->size();
		break;
	}
	// TODO
}

void StudentTextEditor::del() {
	// If valid character
	if (m_col < m_currLine->size()) {
		getUndo()->submit(Undo::DELETE, m_row, m_col, (*m_currLine)[m_col]);
		m_currLine->erase(m_currLine->begin() + m_col);
	}
	// If after last character, not on last line (NOTE: Will submit '\0'
	else if ((m_row + 1) < m_lines.size()) {
		join();
		getUndo()->submit(Undo::JOIN, m_row, m_col);
	}

	// TODO: Tell undo to track delete
}

void StudentTextEditor::join() {
	std::string s1 = *m_currLine;
	m_currLine = m_lines.erase(m_currLine);
	*m_currLine = s1 + *m_currLine;
}

void StudentTextEditor::backspace() {
	// If valid character
	if (m_col > 0) {
		--m_col;
		getUndo()->submit(Undo::DELETE, m_row, m_col, (*m_currLine)[m_col]);
		m_currLine->erase(m_currLine->begin() + m_col);
	}
	// If at first character of line/line is empty AND not on first line
	else if (m_row > 0){
		--m_row;
		--m_currLine;
		m_col = m_currLine->length();
		join();
		getUndo()->submit(Undo::JOIN, m_row, m_col);
	}
	// TODO
}

void StudentTextEditor::insert(char ch) {
	if (ch == '\t') {
		getUndo()->submit(Undo::INSERT, m_row, m_col + 1, '\t');
		m_currLine->insert(m_col, 4, ' ');
		m_col += 4;
	}
	else {
		m_currLine->insert(m_col, 1, ch);
		++m_col;
		getUndo()->submit(Undo::INSERT, m_row, m_col, ch);
	}
	// TODO: Tell undo to track insertion 
}

void StudentTextEditor::enter() {
	getUndo()->submit(Undo::SPLIT, m_row, m_col);
	split();
	// TODO: Tell undo to track split
}

void StudentTextEditor::split() {
	std::string s1 = m_currLine->substr(0, m_col);
	std::string s2 = m_currLine->substr(m_col);
	*m_currLine = s2;
	m_currLine = m_lines.insert(m_currLine, s1);
	++m_currLine;
	++m_row;
	m_col = 0;
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_row;
	col = m_col;
	// TODO
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if (startRow < 0 || numRows < 0 || startRow > m_lines.size()) return -1;
	lines.clear();
	// Traverse to starting row
	std::list<std::string>::const_iterator line = moveToLine(startRow);

	// Add to lines
	int n = 0;
	for (; line != m_lines.end() && n < numRows; ++line) {
		lines.push_back(*line);
		++n;
	}
	return n;
	// TODO
}

std::list<std::string>::iterator StudentTextEditor::moveToLine(int row) const {
	std::list<std::string>::iterator line = m_currLine;
	int dist = abs(m_row - row);
	if (m_row > row) { // travel backwards
		for (int i = 0; i < dist; ++i) {
			--line;
		}
	}
	else { // travel forwards
		for (int i = 0; i < dist; ++i) {
			++line;
		}
	}
	return line;
}

void StudentTextEditor::undo() {
	int row, col, count;
	std::string text;
	Undo::Action action = getUndo()->get(row, col, count, text);
	switch (action) {
	case Undo::Action::INSERT:
		moveCurrToPos(row, col);
		m_currLine->insert(col, text);
		break;
	case Undo::Action::DELETE:
		moveCurrToPos(row, col);
		m_currLine->erase(col, count);
		break;
	case Undo::Action::SPLIT:
		moveCurrToPos(row, col);
		split();
		moveCurrToPos(row, col); // move cursor back after performing the split
		break;
	case Undo::Action::JOIN:
		moveCurrToPos(row, col);
		join();
		break;
	case Undo::Action::ERROR:
		return; // do nothing
	}
}

void StudentTextEditor::moveCurrToPos(int row, int col) {
	m_currLine = moveToLine(row);
	m_row = row;
	m_col = col;
}