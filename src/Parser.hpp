#ifndef PARSER

#define PARSER

#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#define STDIN 0
#define PIPE_MAX 4096
#define delimiters " 1234567890!@#$%^&*()`~-_=+[{]}|:;'/?.>,<\\\"\n\t\r"

class Parser {
public:
	//constructors
	Parser(void);
	Parser(int fd);
	//member variables
	int read_fd;
	//the queue
	std::queue<string*> out_queue;
	//member functions
	bool hasNext();
	string readPipeLine(FILE *file);
private:
	void init(); //no constructor delegation in c++   :(
};
#endif