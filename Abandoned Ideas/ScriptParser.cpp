#include "ScriptParser.h"

Document::Document(const char* filename) {
	std::ifstream in(filename);
	char* line = new char[100];	//Line limit is 100 for convenience
	while (in.peek() != EOF) {
		in.getline(line, 100);
		if (line[0] != '\0')
			mLines.push_back(new Line(line));
	}
	delete[] line;
}

Document::~Document() {
	for (unsigned int i = 0; i < mLines.size(); i++)
		delete mLines[i];
	mLines.clear();
}

Line::Line(char* raw) {
	int i = 0, off = 0;
	char* buf = new char[100];
	while (raw[i] != '\0') {
		if (raw[i] != ' ') {
			buf[i - off] = raw[i];
		}
		else {
			if (buf[0] != '\0') {
				buf[i - off] = '\0';
				mTokens.push_back(new Token(buf, i - off));
			}
			buf[0] = '\0';
			off = i + 1;
		}
		i++;
	}
	delete[] buf;
}

Line::~Line() {
	for (unsigned int i = 0; i < mTokens.size(); i++)
		delete mTokens[i];
	mTokens.clear();
}

Token::Token(char* raw, int length) {
	mString = new char[length + 1];
	for (int i = 0; i < length + 1; i++)
		mString[i] = raw[i];
}

Token::~Token() {
	delete[] mString;
}
