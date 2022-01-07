#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}
//This method must run in O(1) time
StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	m_cCol = 0;
	m_cRow = 0;
	m_undo = undo;
	m_text.push_back("");
	m_line = m_text.begin();
}

//. This method must run in O(N) time where N is the number of
//lines in the file currently being edited.
StudentTextEditor::~StudentTextEditor()
{
	for (std::list<std::string>::iterator it = m_text.begin(); it != m_text.end();) {
		it = m_text.erase(it);
	}
	//so do not free m_undo's memory, do I still need to set undo to nullptr?
	m_undo = nullptr;
}

//This method must run in O(M+N+U)
//time where M is the number of characters in the editor currently being edited, N is the
//number of characters in the new file being loaded, and U is the number of items in the
//stack.
bool StudentTextEditor::load(std::string file) {
	std::ifstream infile(file);    // infile is a name of our choosing
	if (!infile)		        // Did opening the file fail?
	{
		return false;
	}
	//If you were already editing an existing file and the specified file can be
	//loaded, then the old contents of the text editor must be reset
	//this is U+M
	reset();

	// this is N
	std::string s;
	// getline returns infile; the while tests its success/failure state
	while (getline(infile, s))
	{
		//check if the last character is '\r'
		//TODO: strip out '\r'
		//infile.ignore(10000, '\r');
		if (s.size()!=0 && s.at(s.size() - 1) == '\r') {
			s = s.erase(s.size() - 1);
		}
		m_text.push_back(s);
	}
	//reset the current editing position to the beginning row / column of the file and return true
	m_cRow = 0;
	m_line = m_text.begin();
	m_cCol = 0;
	return true;

}

//This method must run in O(M) time where M is
//the number of characters in the editor currently being edited.
bool StudentTextEditor::save(std::string file) {
	std::ofstream outfile(file);   // outfile is a name of our choosing.
	if (!outfile)		   // Did the creation fail?
	{
		return false;
	}
	//this is M
	for (std::list<std::string>::iterator it = m_text.begin(); it != m_text.end(); it++) {
		//not sure about endl, try after implement insert
		outfile << *it << std::endl;
	}
	outfile << std::endl;
	return true;
}


void StudentTextEditor::reset() {
	m_cRow = 0;
	m_cCol = 0;
	m_line = m_text.begin();
	//clear the undo
	m_undo->clear();
	//erase all the lines
	for (std::list<std::string>::iterator it = m_text.begin(); it != m_text.end();) {
		it = m_text.erase(it);
	}
}


void StudentTextEditor::move(Dir dir) {
	if (dir == Dir::UP) {
		if (m_cRow == 0)
			return;
		m_cRow--;
		m_line--;
		if (m_line->empty()) {
			m_cCol = 0;
		}
		else if (m_cCol > m_line->size()) {
			m_cCol = m_line->size();
		}
		return;
	}
	else if (dir == Dir::DOWN) {
		// it points to the last line
		std::list<std::string>::iterator it = m_text.end();
		it--;
		//if the cursor is in the last line, simply return
		if (m_line == it)
			return;
		m_cRow++;
		m_line++;
		if (m_line->empty()) {
			m_cCol = 0;
		}
		else if (m_cCol > m_line->size()) {
			m_cCol = m_line->size();
		}
		return;
	}
	else if (dir == Dir::LEFT) {
		// if in the first line and in the first char
		if (m_cRow ==0 && m_cCol == 0)
			return;
		//if in any other line's first char
		if (m_cCol == 0) {
			m_line--;
			m_cCol = m_line->size();
			m_cRow--;
			return;
		}
		//if not in the first char
		m_cCol--;
		return;
	}
	else if (dir == Dir::RIGHT) {
		// it points to the last line
		std::list<std::string>::iterator it = m_text.end();
		it--;
		//if the cursor is in the last line and the last char, simply return
		if (m_line == it&&m_cCol >= it->size())
			return;
		//if the cursor is at the last char of any line, excet the last line
		if (m_cCol == m_line->size()) {
			m_line++;
			m_cCol = 0;
			m_cRow++;
			return;
		}
		// if not in the last char
		m_cCol++;
		return;
	}
	else if (dir == Dir::HOME) {
		//moves the editing position to the first character on the current line
		m_cCol = 0;
			return;
	}
	else if (dir == Dir::END) {
		//moves the editing position just after the last character on the current line
	
		m_cCol = m_line->size();
		return;
	}
}
//Deletion of a character in the middle of a line must run in O(L) time where L is the length
//of the line of text containing the current editing position.Deletion at the end of a line
//resulting a merge must run in O(L1 + L2) where L1 is the length of the current line of text
//at the editing positionand L2 is the length of the next line in the editor.This command
//must not have a runtime that depends on the number of lines being edited!
void StudentTextEditor::del() {
	//if delete a char at the end
	if (m_cCol == m_line->size()) {
		//if it is the last line, no 2nd line to merge
		std::list<std::string>::iterator it = m_text.end();
		it--;
		if (m_line == it)
			return;
		//set it to the next line
		it = m_line;
		it++;
		//step 1; copy the 2nd line to the first line
		//dont know if this meet the time complex requirement
		*m_line = *m_line + *it;
		//step 2; erase the 2nd line 
		m_text.erase(it);
		//step 3, submit to undo
		m_undo->submit(Undo::Action::JOIN, m_cRow, m_cCol);
		return;
	}
	//if delete a character that is not at the end
	std::string copyStr = *m_line;
	char undo = copyStr[m_cCol];
	*m_line = copyStr.substr(0, m_cCol) + copyStr.substr(m_cCol + 1);
	//submit undo
	m_undo->submit(Undo::Action::DELETE, m_cRow, m_cCol,undo);
	return;
}

//Backspacing that does not result in a merge must run in O(L) time where L is the length
//of the line of text containing the current editing position.Backspacing at the front of a
//line resulting in a merge with the previous line must run in O(L1 + L2) where L1 is the
//length of the line of text containing the current editing positionand L2 is the length of the
//previous line in the editor.This command must not have a runtime that depends on the
//number of lines being edited!
void StudentTextEditor::backspace() {
	//backspaing when col >0
	if (m_cCol > 0) {
		std::string copyStr = *m_line;
		//delete that specific char
		char undo = copyStr[m_cCol - 1];
		*m_line = copyStr.substr(0, m_cCol-1) + copyStr.substr(m_cCol);
		m_cCol--;
		//submit the undo
		m_undo->submit(Undo::Action::DELETE, m_cRow, m_cCol, undo);
		return;
	}
	//if in the first line, no previous line to merge
	if (m_line == m_text.begin())
		return;

	//merge two lines
	std::list<std::string>::iterator it = m_line;
	it--;
	int curPos = it->size();
	*it = *it + *m_line;
	//delete the 2nd line
	m_text.erase(m_line);
	m_line = it;
	m_cRow--;
	m_cCol = curPos;
	m_undo->submit(Undo::Action::JOIN, m_cRow, m_cCol);
	return;

}

//must run in O(L) time where L is the length of the line
//of text containing the current editing position.
//does this function meet the time complexity requirement?
void StudentTextEditor::insert(char ch) {
	
	//if insert a char in the middile
	std::string save = *m_line;
	if (ch == '\t') {
		for (int i = 0; i < 4; i++) {
			std::string s = " ";
			*m_line = save.insert(m_cCol, s);
			m_cCol++;
			m_undo->submit(Undo::Action::INSERT, m_cRow, m_cCol, ' ');
		}
	}
	else {
		std::string s;
		s += ch;
		*m_line = save.insert(m_cCol, s);
		m_cCol++;
		m_undo->submit(Undo::Action::INSERT, m_cRow, m_cCol, ch);
	}
	return;
}
//Using the enter command anywhere on a line must run in O(L) time where L is the length
//of the line of text containing the current editing position.This command must not have a
//runtime that depends on the number of lines being edited
void StudentTextEditor::enter() {
	//if hit enter when the cursor is at the bigin of a line
	if (m_cCol == 0) {
		//submit undo
		m_undo->submit(Undo::Action::SPLIT, m_cRow, m_cCol);
		//insert a new line at this line, make the original line to the next line
		m_text.insert(m_line, "");
		m_cRow++;
		m_cCol = 0;
		return;
	}
	//if hit enter when the cursor is past the last char of a line
	if (m_cCol == m_line->size()) {
		//insert a empty string line to the next line
		//not sure if need a new line char
		//it points to the next line
		std::list<std::string>::iterator it = m_line;
		it++;
		m_undo->submit(Undo::Action::SPLIT, m_cRow, m_cCol);
		//insert a new line in the next line
		m_text.insert(it, "");
		m_cRow++;
		m_cCol = 0;
		m_line++;
		return;
	}
    // if hit enter when the cursor is at the middle
	std::string copyStr = *m_line;
	m_undo->submit(Undo::Action::SPLIT, m_cRow, m_cCol);
	//create a new line with the characters after current column
	m_text.insert(m_line, copyStr.substr(0, m_cCol));
	//make the current line(become the next line), with appropriate size
	*m_line = copyStr.substr(m_cCol);
	m_cRow++;
	m_cCol = 0;
	return;

}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_cRow;
	col = m_cCol;
	return;
}

//This operation must run in O(abs(current row number - startRow) + numRows*L) time,
//where L is the average line length.The operation’s runtime must not depend on the
//number of lines being edited.
int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	//If startRow or numRows is negative, or if startRow is greater than the number of lines in the text,
	//return -1
	//time complexity is O(1)
	if (startRow < 0 || numRows<0 || startRow>m_text.size()) {
		return -1;
	}
	//clearing any previous data that was in the lines variable before adding the requested lines
	//does this fit the time complexity requirement?
	for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end();) {
		it = lines.erase(it);
	}
	// if startRow is equal to the number of lines in the text, the lines parameter will be empty upon return.
	if (startRow == m_text.size())
		return 0;
	//store the number of rows to retrieve
	int rowStore = m_text.size() - startRow;
	if (numRows < rowStore) {
		rowStore = numRows;
	}

	//make a iterator to point to the startRow
	std::list<std::string>::const_iterator it = m_text.begin();
	for (int i = 0; i < startRow; i++) {
		it++;
	}
	//the available lines must be added to the lines parameter
	for (int i = 0; i < rowStore; i++) {
		lines.push_back(*it);
		it++;
	}
	return rowStore;

}

//This method must run in a time proportional to the nature of the undo operation. For
//example, if the undo is of a deleted character, then its runtime must be the cost to reinsert the deleted character into the editor.
//If the undo is of a line break, then the runtime
//must be the cost to re - merge the line that was broken into two parts, etc.
void StudentTextEditor::undo() {
	int count = 0;
	std::string str = "";
	Undo::Action temp = m_undo->get(m_cRow, m_cCol, count, str);
	//having a iterator to point to the m_col that being to
	m_line = m_text.begin();
	for (int i = 0; i < m_cRow; i++) {
		m_line++;
	}

	//if undo a insert action
	if (temp == Undo::Action::DELETE) {
			m_cCol = m_cCol + count - 1;
		//using backspacing function to do this job
		for (int i = 0; i < count; i++) {
			if (m_cCol > 0) {
				std::string copyStr = *m_line;
				//delete that specific char
				*m_line = copyStr.substr(0, m_cCol - 1) + copyStr.substr(m_cCol);
				m_cCol--;
			}
			else {
				m_line->erase(0);
			}
		}
		return;
	}

	//if undo is a delete action
	if (temp == Undo::Action::INSERT) {
		int fCol = m_cCol;
		for (int i = 0; i < str.size(); i++) {
			std::string save = *m_line;
			std::string s;
			s += str[i];
			*m_line = save.insert(m_cCol, s);
			m_cCol++;

		}
		m_cCol = fCol;
	}

	if (temp == Undo::Action::JOIN) {
		//if it is the last line, no 2nd line to merge
		std::list<std::string>::iterator it = m_line;
		it++;
		//step 1; copy the 2nd line to the first line
		//dont know if this meet the time complex requirement
		*m_line = *m_line + *it;
		//step 2; erase the 2nd line 
		m_text.erase(it);
		//step 3, submit to undo
	}
	if (temp == Undo::Action::SPLIT) {
		// if hit enter when the cursor is at the middle
		std::string copyStr = *m_line;
		//create a new line with the characters after current column
		m_text.insert(m_line, copyStr.substr(0, m_cCol));
		//make the current line(become the next line), with appropriate size
		*m_line = copyStr.substr(m_cCol);
		m_line--;
		return;
	}
}
