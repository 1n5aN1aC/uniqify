// main.cpp : main project file.

#include "stdinc.h"

using namespace std;

int workers = 10;

//Method Declarations
bool kill(string message);
bool error(string message);
string readPipeLine(FILE *file);

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
	
	bool quit = false;
	while (!quit) {
		
		int filed[2];

		if(pipe(filed) == -1)
			kill("pipe creation error.");

		switch (fork()) {
		case -1:
			kill("error forking!");
		case 0: //child
			if (close(filed[1]) == -1)
				kill("close - child");

			for (;;) {
				numRead = read(pfd[0], buf, BUF_SIZE);
				if (numRead == -1)
					kill("error reading");
				if (numRead == 0)
					break;
				if (write(STDOUT_FILENO, buf numRead) != numRead)
					kill("partial read/write failure");
			}

			break;  //leave for safety
		default: //parent
			if (close(filed[0]) == -1)
				kill("close - parent");
			//write to pipe
			break;
		}

		quit = true;
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