#include "StudentUndo.h"
#include <stack>

Undo* createUndo()
{
	return new StudentUndo;
}

StudentUndo::ID::ID() {
	cUndo = "";
	m_id = Action::ERROR;
	m_row = 0;
	m_col = 0;
	count = 0;
}

//This method must run in O(1) time in the average case, understanding that it may
//increase to O(current line length where the operation occurred) infrequently.
//but my time complexity is o(1), does this violates??
void StudentUndo::submit(const Action action, int row, int col, char ch) {
	if (!m_undoStack.empty()) {
		ID* last = &m_undoStack.top();
		if (action == last->m_id) {
		//del
			if (action == Undo::Action::DELETE && row == last->m_row && col == last->m_col) {
				last->cUndo += ch;
				last->m_row = row;
				return;
			}

			//backspace
			if (action == Undo::Action::DELETE && row == last->m_row && col + 1 == last->m_col) {
				last->cUndo = ch + last->cUndo;
				last->m_row = row;
				last->m_col = col;
				return;
			}

			//insert
			if (action == Undo::Action::INSERT) {
				last->count++;
				last->m_row = row;
				last->m_col = col;
				return;
			}


		}
	}
	//for other information
	ID newId;
	newId.m_id = action;
	newId.count = 1;
	newId.m_row = row;
	newId.m_col = col;
	newId.cUndo += ch;
	m_undoStack.push(newId);
	return;

}

//This method must run in O(1) time in the average case, understanding that it may
//increase to O(length of deleted characters that need to be restored) for restoration cases.
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) {

	if (m_undoStack.empty()) {
		//If the undo stack was empty when this
			//method was called
		return Undo::Action::ERROR;
	}
	
	ID* temp = &m_undoStack.top();
	//delete the top item from the stack
	m_undoStack.pop();
	row = temp->m_row;
	col = temp->m_col;
	count = 1;
	text = "";
	//check if the undo submit is insert
	if (temp->m_id == Undo::Action::INSERT) {
		count = temp->count;
		col -= count;
		return Undo::Action::DELETE;
	}
 //if the action is delete
	if (temp->m_id == Undo::Action::DELETE) {
		text = temp->cUndo;
		return Undo::Action::INSERT;
	}
	//if the action is to split the line
	if (temp->m_id == Undo::Action::SPLIT) {
		return Undo::Action::JOIN;
	}

	//if the action is to join the line
	if (temp->m_id == Undo::Action::JOIN) {
		return Undo::Action::SPLIT;
	}

	return Action::ERROR;
}

void StudentUndo::clear() {
	//clear all elements in a stack
	for (int i = 0; i < m_undoStack.size(); i++) {
		m_undoStack.pop();
	}
}
