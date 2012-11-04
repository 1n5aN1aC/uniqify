// main.cpp : main project file.

#include "stdinc.h"		//global includes here

using namespace std;	//namespace for ease of use

//declare global variables
int workers = 10;

//Method Declarations
bool kill(string message);
bool error(string message);
string readFileLine(FILE* file);
void writeFileLine(string strIn, FILE* fToWrite);
FILE* openFD(int fd, string mode);
void closeFile(FILE* fileToClose);
void closeFD(int fd);
int* makePipe();
//change fd
//wait / waitpid
void sort();

int main(int argc, char** argv)
{
	if (argc < 2) {
		cout << "Quantity Not Specified, So Assuming 10 Working Processess" << endl;
		workers = 10;
	}
	else if (argc > 3) {
		cout << HELP << endl << "I can't handle all this awesomness!\n" << "Too many arguments!" << endl;
		kill("Invalid use of uniqify.");
	}
	else
		workers = atoi (argv[1]);

	//Create 2-d filedescriptor array.
	//up to 7(0-6) for purposes of also storing process -> consolidator.
	//number 7 (6) is the pid of the child.
	int filed[workers][7];

	//test loop
	for (int i=0; i < workers; i++) {
		cout << "derp " << endl;
	}

	for (int i=0; i < workers; i++) {
		int temp = fork();
		if (temp == -1)
			kill("error forking");
		if (temp == 0) {
			//child code
			cout << "child " << i << " ran" << endl;
			exit(0);
		}
		else {
			//save child pid into last element of master fd array for later use.
			filed[i][6] = temp;
			cout << filed[i][6] << endl;
		}
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
string readFileLine(FILE *file) {
	//assuming FILE is valid if it gets called into here.
	char *buff = (char*)malloc(PIPE_MAX);

	if(fgets(buff,PIPE_MAX,file) == "eof")
		return std::string("eof");

	string temp = string(buff);
	free(buff);
	return temp;
}
void writeFileLine(string strIn, FILE* fToWrite);
FILE* openFD(int fd, string mode) {
	FILE* file;
	file = fdopen(fd, mode.c_str());
	if (file == NULL)
		kill("Error opening FDas a file.");
	return file;
}
void closeFile(FILE* fileToClose);
void closeFD(int fd) {
	if (close(fd) == -1)
		kill("Error closing file.");
}
int* makePipe();
//change fd
//wait / waitpid
void sort() {
	execlp("sort","","");
}