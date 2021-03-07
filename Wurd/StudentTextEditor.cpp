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
	std::string s;
	while (getline(infile, s)) {
		if (s.back() == '\r') s.erase(s.end() - 1);
		m_lines.push_back(s);
	}
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
}

void StudentTextEditor::move(Dir dir) {
	switch (dir)
	{
	case Dir::UP:
		if (m_row > 0) {
			--m_row;
			--m_currLine;
			m_col = 0;
		}
		break;
	case Dir::DOWN:
		if (m_row + 1 < m_lines.size()) {
			++m_row;
			++m_currLine;
			m_col = 0;
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
		else if (m_row + 1 < m_lines.size()) {
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
	else if (m_row + 1 < m_lines.size()) {
		std::string s1 = *m_currLine;
		m_currLine = m_lines.erase(m_currLine);
		*m_currLine = s1 + *m_currLine;
		getUndo()->submit(Undo::JOIN, m_row, m_col);
	}

	// TODO: Tell undo to track delete
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
		m_col = m_currLine->size();
		std::string s1 = *m_currLine;
		m_currLine = m_lines.erase(m_currLine);
		*m_currLine = s1 + *m_currLine;
		getUndo()->submit(Undo::JOIN, m_row, m_col);
	}
	// TODO
}

void StudentTextEditor::insert(char ch) {
	if (ch == '\t') {
		m_currLine->insert(m_col, 4, ' ');
		m_col += 4;
	}
	else {
		m_currLine->insert(m_col, 1, ch);
		++m_col;
	}
	getUndo()->submit(Undo::INSERT, m_row, m_col, ch);
	// TODO: Tell undo to track insertion 
}

void StudentTextEditor::enter() {
	getUndo()->submit(Undo::SPLIT, m_row, m_col);
	std::string s1 = m_currLine->substr(0, m_col);
	std::string s2 = m_currLine->substr(m_col);
	*m_currLine = s2;
	m_currLine = m_lines.insert(m_currLine, s1);
	++m_currLine;
	++m_row;
	m_col = 0;
	// TODO: Tell undo to track split
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
	std::list<std::string>::const_iterator line = m_currLine;
	int dist = abs(m_row - startRow);
	if (m_row > startRow) { // travel backwards
		for (int i = 0; i < dist; ++i) {
			--line;
		}
	}
	else { // travel forwards
		for (int i = 0; i < dist; ++i) {
			++line;
		}
	}

	// Add to lines
	int n = 0;
	for (; line != m_lines.end() && n < numRows; ++line) {
		lines.push_back(*line);
		++n;
	}
	return n;
	// TODO
}

void StudentTextEditor::undo() {
	int row, col, count;
	std::string text;
	Undo::Action action = getUndo()->get(row, col, count, text);
}
