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
	struct ID
	{
		ID();
		std::string cUndo;
		Undo::Action m_id;
		int m_row;
		int m_col;
		int count;
	};
	std::stack<ID> m_undoStack;
};

#endif // STUDENTUNDO_H_
