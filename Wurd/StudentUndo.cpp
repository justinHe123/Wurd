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
		Item prev = m_actions.top();
		// Check if curr delete, prev del (previous delete was the same position)
		if (action == Action::DELETE && prev.action == Action::DELETE && prev.row == row && prev.col == col) {
			m_actions.top().text += ch;
		}
		// Check if curr delete, prev backspace (previous delete was same row, col + 1)
		else if (action == Action::DELETE && prev.action == Action::DELETE && prev.row == row && prev.col == col + 1) {
			m_actions.top().text = ch + prev.text;
			m_actions.top().col = col;
		}
		// Check if curr insert, prev insert (previous insert was same row, col - 1)
		else if (action == Action::INSERT && prev.action == Action::INSERT && prev.row == row && prev.col == col - 1) {
			m_actions.top().text += ch;
			m_actions.top().col = col;
		}
		// Not a batchable operation
		else {
			Item item(action, row, col, ch);
			m_actions.push(item);
		}
	}
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
	// send the opposite action back
	switch (action) {
	case Action::INSERT:
		count = item.text.size();
		col -= count; // decrement col by count to get to start of where to delete
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
	default:
		return Action::ERROR;
		break;
	}
}

void StudentUndo::clear() {
	while (!m_actions.empty()) m_actions.pop(); // clear the stack
}
