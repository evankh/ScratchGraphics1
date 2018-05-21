#ifndef __EKH_SCRIPT_PARSER__
#define __EKH_SCRIPT_PARSER__
#include <vector>
#include <fstream>

char* keywords[] = {
	"state",
	"set",
	"add",
	"animation",
	"position",
	"velocity",
	"acceleration",
	"input",
	"collision",
	"timer"
};

class Token {
private:
	char* mString;
public:
	Token(char* raw, int length);
	~Token();
};

class Line {
private:
	std::vector<Token*> mTokens;
public:
	Line(char* raw);
	~Line();
};

class StateMachine;

class Document {
private:
	std::vector<Line*> mLines;
public:
	Document(const char* filename);
	~Document();
	StateMachine parse();
};

#endif//__EKH_SCRIPT_PARSER__