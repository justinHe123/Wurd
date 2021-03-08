#include "StudentUndo.h"

#include <cctype>

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	// Stack is empty, nothing to batch
	if (m_actions.empty()) {
		Item item(action, row, col, ch);
		m_actions.push(item);
	}
	else {
		Item curr = m_actions.top();
		// Check if del (previous delete was the same position
		if (curr.action == Action::DELETE && curr.row == row && curr.col == col) {
			m_actions.top().text += ch;
		}
		// Check if backspace (previous delete was same row, col + 1)
		else if (curr.action == Action::DELETE && curr.row == row && curr.col == col + 1) {
			m_actions.top().text = ch + curr.text;
			m_actions.top().col = col;
		}
		// Check if insert (previous insert was same row, col - 1) AND previous character was part of a word
		else if (curr.action == Action::INSERT && curr.row == row && curr.col == col - 1 && isValid(curr.text[curr.text.length() - 1])) {
			m_actions.top().text += ch;
			m_actions.top().col = col;
		}
		// Not a batchable operation
		else {
			Item item(action, row, col, ch);
			m_actions.push(item);
		}
	}
	// TODO: Figure out batching
}

bool StudentUndo::isValid(char ch) const {
	return isalpha(ch) || ch == '\'';
}

StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) {
	if (m_actions.empty()) return Action::ERROR;  // TODO
	Item item = m_actions.top();
	m_actions.pop();
	row = item.row;
	col = item.col;
	count = 1;
	text = "";
	Action action = item.action;
	switch (action) {
	case Action::INSERT:
		count = item.text.size();
		col -= count;
		return Action::DELETE;
		break;
	case Action::SPLIT:
		return Action::JOIN;
		break;
	case Action::DELETE:
		text = item.text;
		return Action::INSERT;
		break;
	case Action::JOIN:
		return Action::SPLIT;
		break;
	}
	// TODO
}

void StudentUndo::clear() {
	while (!m_actions.empty()) m_actions.pop();
	// TODO
}
