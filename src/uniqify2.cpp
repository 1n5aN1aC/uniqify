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
	else if (argc > 2) {
		cout << HELP << endl << "I can't handle all this awesomness!\n" << "Too many arguments!" << endl;
		kill("Invalid use of uniqify.");
	}
	else
		workers = atoi (argv[1]);

	//Create 2-d filedescriptor arrays.
	int procInFD[workers][2];	//File Descriptors of pipe from parser to children
	FILE* procInFile[workers];	//array of pipes opened as files
	int procOutFD[workers][2];	//File descriptors of pipe from children to sorter
	int procPID[workers];		//pid of children

	for (int i=0; i < workers; i++) {
		//Create a pipe for each child proccess.
		if (pipe(procInFD[i]) == -1)
			kill("error making pipes.");

		if (pipe(procOutFD[i]) == -1)
			kill("error making pipes.");

		int temp = fork();
		if (temp == -1)
			kill("error forking");
		if (temp == 0) {
			//cout << "child " << i << " ran and got fds: " << filed[i][0] << " " << filed[i][1] << endl;	//test debug code

			dup2(procInFD[i][0], 0);	//dupe pipe from distributer to child proccess stdin
			closeFD(procInFD[i][1]);	//close unused File Descriptor
			//dup2(procOutFD[i][1], 1);		//dupe child proccess cout to pipe to final sorter

			sort();					//exec() the sorting proccess

			return 0;				//kill ourself.
		}
		else {
			//save child pid into last element of master fd array for later use
			procPID[i] = temp;

			//FILE* derpIn = openFD(procInFD[i][0], "r");	//open as file
			//char buf[4100] = "derp aerp";
			//fputs(buf,derpIn);

			//FILE* derpOut = openFD(procInFD[i][1], "w");	//open as file
			//char buf2[4100];
			//fgets(buf2,PIPE_MAX,derpOut);
			//cout << buf;

			//cout test code.
			//cout << filed[i][4] << endl;
		}
	}


	for (int i=0;i < workers;i++) {
		procInFile[i] = openFD(procInFD[i][1], "w");
	}
	int currPipe = 0;

	while (true) {
		FILE* CIN = fdopen(0, "r");
		char buff[4100];
		if(fgets(buff, PIPE_MAX, CIN) == NULL)
			break;
	
		//split the line read into seperate strings
		vector<string> splitAr;
		vector<string> buffer;
		boost::split(splitAr,buff,boost::is_any_of(delimiters));
		BOOST_FOREACH(string s, splitAr){
			if(!s.empty()){
				boost::algorithm::to_lower(s);
				buffer.push_back(s);
			}
		}

		//Write to pipes-  round-robbin style.
		BOOST_FOREACH(string thisWord, buffer)
			{
				fputs(thisWord.c_str(),procInFile[currPipe]);
				currPipe++;
				if (currPipe > workers-1)
					currPipe = 0;
			}

	}

	//Close Open Files (pipes)
	for (int i=0; i<workers; i++) {
		closeFile(procInFile[i]);
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
		kill("Error opening FD as a file.");
	return file;
}
void closeFile(FILE* fileToClose) {
	if (fclose(fileToClose) == -1)
		kill("Error closing file");
}
void closeFD(int fd) {
	if (close(fd) == -1)
		kill("Error closing file descriptor.");
}
int* makePipe();
//change fd
//wait / waitpid
void sort() {
	execlp("sort","","");
}