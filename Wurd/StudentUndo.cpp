#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	Item item(action, row, col, ch);
	m_actions.push(item);
	// TODO: Figure out batching
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
		count = text.size();
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
		return Action::JOIN;
		break;
	}
	// TODO
}

void StudentUndo::clear() {
	while (!m_actions.empty()) m_actions.pop();
	// TODO
}
