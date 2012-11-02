#include "Parser.hpp"

using namespace std;

Parser::Parser(void) {
	read_fd = STDIN;

}
Parser::Parser(int fd) {
	read_fd = fd;
}
void Parser::init() {

}

string readPipeLine(FILE *file) {
	//assuming FILE is valid if it gets called into here.
	char *buf = (char*)malloc(PIPE_MAX);

	if(fgets(buf,PIPE_MAX,file) == "eof")
		return std::string("eof");

	string temp = string(buff);
	free(buff);
	return temp;
}