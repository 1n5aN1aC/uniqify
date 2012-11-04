// main.cpp : main project file.

#include "stdinc.h"		//global includes here

using namespace std;	//namespace for ease of use

//declare global variables
int workers = 10;

//Method Declarations
bool kill(string message);
bool error(string message);
string readPipeLine(FILE *file);
FILE* openFD(int fd, string mode);
void closeFD(int fd);

int main(int argc, char** argv)
{
	if (argc < 2) {
		cout << "Assuming 10 Working Processess" << endl;
		workers = 10;
	}
	else if (argc > 3) {
		cout << HELP << endl << "I can't handle all this awesomness!\n" << "Too many arguments!" << endl;
		kill("Invalid use of uniq.");
	}
	else
		workers = atoi (argv[1]);
	
	cout << workers;
	
	for (int i=0; i < workers; i++) {
		cout << "derp ";
	}

	int filed[2];

	if(pipe(filed) == -1)
		kill("pipe creation error.");

	switch (fork()) {
	case -1:
		kill("error forking!");
	case 0: //child
		if (close(filed[1]) == -1)
			kill("close - child");

		for (;PIPE_MAX;filed[0]) {
			char buf[4100];
			//int numRead = read(filed[0], buf, PIPE_MAX);
			FILE* childIn = openFD(filed[0], "r");
			fgets(buf,PIPE_MAX,childIn);
			//if (numRead == NULL)
			//	kill("error reading");
			//if (numRead == 0)
			//	break;
			//if (write(STDOUT_FILENO, buf, numRead) != numRead)
			//	kill("partial read/write failure");
		}

		write (STDOUT_FILENO, "\n", 1);
		if (close(filed[0] == -1))
			kill("close error");
		_exit(EXIT_SUCCESS);
	default: //parent
		if (close(filed[0]) == -1)
			kill("close - parent");
			
		if (write(filed[1], argv[1], strlen(argv[1])) != strlen(argv[1]))
			kill("parent partial read/write fail");

		if (close(filed[1]) == -1)
			kill("parent close error");
		wait(NULL);
		exit(EXIT_SUCCESS);
	}

	return 0;
}

bool kill(string message) { //Ends program with error.
	perror(message.c_str());
	exit(-1);
	return false;
}
bool error(string message) {
	perror(message.c_str());
	return false;
}
string readPipeLine(FILE *file) {
	//assuming FILE is valid if it gets called into here.
	char *buff = (char*)malloc(PIPE_MAX);

	if(fgets(buff,PIPE_MAX,file) == "eof")
		return std::string("eof");

	string temp = string(buff);
	free(buff);
	return temp;
}
FILE* openFD(int fd, string mode) {
	FILE* file;
	file = fdopen(fd, mode.c_str());
	if (file == NULL)
		kill("Error opening FDas a file.");
	return file;
}
void closeFD(int fd) {
	if (close(fd) == -1)
		kill("Error closing file.");
}