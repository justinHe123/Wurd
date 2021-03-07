#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"

#include <stack>
#include <string>
class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct Item {
		Item(Action action, int row, int col, char ch) : action(action), row(row), col(col), text(1, ch) {}
		Action action;
		int row;
		int col;
		std::string text;
	};
	std::stack<StudentUndo::Item> m_actions;
};

#endif // STUDENTUNDO_H_
