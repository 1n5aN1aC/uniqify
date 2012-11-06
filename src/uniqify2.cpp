// main.cpp : main project file.

#include "stdinc.h"		//global includes here

using namespace std;	//namespace for ease of use

//declare global variables
int workers = 10;

//Method Declarations
bool kill(string message);
bool error(string message);
string readFileLine(FILE* file);
FILE* openFD(int fd, string mode);
void closeFile(FILE* fileToClose);
void closeFD(int fd);
void waitForChild();
string charToString(char inArray[]);
string getMergeBuff(int whichBuff, FILE* whatFile);

int main(int argc, char** argv)
{
	//just ran program.  assume 10 working children
	if (argc < 2) {
		cout << "Quantity Not Specified, So Assuming 10 Working Processess" << endl;
		workers = 10;
	}
	//Privided too many arguments.  Comit suicide
	else if (argc > 2) {
		cout << HELP << endl << "I can't handle all this awesomness!\n" << "Too many arguments!" << endl;
		kill("Invalid use of uniqify.");
	}
	//Provided correct arguments.  Store in variable
	else
		workers = atoi (argv[1]);

	//Create 2-d filedescriptor arrays.
	int procInFD[workers][2];	//Array of FDescriptors of pipe from parser to children
	FILE* procInFile[workers];	//Array of pipes opened as files for distributor
	int procOutFD[workers][2];	//Array of DDescriptors of pipe from children to sorter
	FILE* procOutFile[workers];	//Array of pipes opened as files for merger
	int procPID[workers];		//Pid of children

	for (int i=0; i < workers; i++) {
		//Create a pipe for each child from distributor to children (workers)
		if (pipe(procInFD[i]) == -1)
			kill("error making pipes.");
		//Create pipes for use from children (workers) to merger
		if (pipe(procOutFD[i]) == -1)
			kill("error making pipes.");

		//Fork off children
		int temp = fork();
		if (temp == -1)
			kill("error forking"); //error
		if (temp == 0) {
			//The following code was for testing the child code.
			//Encountered major fatal errors IRL xD
			//Read the documentation created with make doc for more info.
			//cout << "child " << i << " ran and got fds: " << procInFD[i][0] << " " << procInFD[i][1] << "\n";	//test debug code

			dup2(procInFD[i][0], 0);	//dupe pipe from distributer to child proccess stdin
			dup2(procOutFD[i][1], 1);	//dupe child proccess stdout to pipe to final sorter

			//closeFD(procInFD[i][0]);	//close unused File Descriptor (Distributor <- Child)
			//closeFD(procInFD[i][1]);	//close unused File Descriptor (Distributor -> Child) [obsolete after dup2]
			//closeFD(procOutFD[i][1]);	//close unused File Descriptor (Child <- Sorter)

			execlp("sort","","");		//exec() the sorting proccess

			//Should never get executed.  Added for ability to sleep at night.
			return 0;					//kill ourself.
		}
		else {
			//Parent (Distributer) Code
			//save child pid into last element of master fd array for later use
			procPID[i] = temp;

			//Close Unused FD's
			//These are currently disabled as they appear to cause issues under certain circumstances
			//Read the documentation created with make doc for more info.
			//closeFD(procInFD[i][0]);	//close unused File Descriptor (Distributor <- Child)
			//closeFD(procOutFD[i][1]);	//close unused File Descriptor (Child <- Sorter)
			//closeFD(procOutFD[i][0]); //(Sorter -> Child) WILL BE USED LATER.  DON'T CLOSE IT!!!!!
		}
	}

	//open write end of distribution->children pipes.
	//
	//Changed the following for loop to a manual for loop to help debugging
	//for (int i=0; i < workers; i++)
	int i = 0;
	while (i < workers)
	{
		//open each master -> child (worker) thread for writing
		procInFile[i] = openFD(procInFD[i][1], "w");
		//Test / Debug code
		cout << "Thread ID: " << i << " file: " << procInFile[i] << " fd: " << procInFD[i][1]<< endl;
		i++;
	}
	int currPipe = 0;	//keeps track of current pipe

	//loop till I say so
	//This loop will read in from STDIN one line, parse it, 
	//and send it down the pipes to the children.
	while (true) {
		//open STDIN for main program for reading
		FILE* CIN = fdopen(0, "r");
		char buff[4100];			//temporary buffer for STDIN reading
		fgets(buff, PIPE_MAX, CIN);	//Read line of STDIN
		if (feof(CIN)) {			//TODO- test past first line
			cout << "break\n\n";	//*mostly* debug code.
			break;
		}
	
		//split the line read into seperate strings
		vector<string> splitAr;						//List of all strings from STDIN line
		vector<string> buffer;						//List of non-empty strings from same
		boost::split(splitAr,buff,boost::is_any_of(DELIMITERS));	//cheat for splitting
		BOOST_FOREACH(string s, splitAr){
			if(!s.empty()){							//if this string is not empty:
				boost::algorithm::to_lower(s);		//make sure Its lower case
				buffer.push_back(s);				//add to the "good" array
			}
		}

		//Write to pipes-  round-robbin style.
		BOOST_FOREACH(string thisWord, buffer) {
			cout << thisWord.c_str() << endl;		//Debug code.  Remove this
			if(fputs(thisWord.c_str(),procInFile[currPipe]) == NULL) {
				cout << "CRUD";						//Was an error
			}
			currPipe++;								//Move on to next pipe next time
			if (currPipe > workers-1)				//Unless we went past our number of pipes
				currPipe = 0;						//If we did, restart from first pipe
			fflush(NULL);							//MAKE SURE ITS *ACTUALLY* IN THE PIPES
		}
	}

	//Close Open Files (pipes)
	//Commented out to *possibly* solve the fatal errors I was having
	for (int i=0; i<workers; i++) {
		//closeFile(procInFile[i]);
		//closeFD(procInFD[i][1]);
	}

	//Fork() off the merger program
	//Here we make an array of c++ strings that is the "next" value from each pipe.
	//We iterate throught hem, find the lowest one with compare() and output it
	//we then refil that "buffer" with the next value from *that* pipe.
	switch ( fork() ) {
	case -1:
		kill("error forking");
	case 0:
	{
		// (Merger Code)
		//open read end of child->Merger pipes.
		for (int i=0;i < workers;i++) {
			procOutFile[i] = openFD(procOutFD[i][0], "r");
		}
		//Initialize array for Individual Child Sorter Buffers.
		string ChildBufs[workers];

		//Iterate through, Filling each of the one-word buffers for the first time.
		//DOES NOT HANDLE LESS THAN (workers) VALUES NEEDS MINOR CHANGES
		for (int i=1;1<workers;i++) {
			ChildBufs[i] = getMergeBuff(i, procOutFile[i]);
		}

		//Begin finding the lowest value word for this run
		while(true) {
			string curLowest = "";					//Curr lowest initialization
			bool flag = true;						//Is this the first check? (yes)
			int pipeITookItFrom;					//Which pipe did we take it from?

			for (int i=0; i<workers; i++) {			//loop through all "buffers"
				if (flag) {							//If this is the first pipe, just assume its lowest
					curLowest = ChildBufs[i];		//so we set the first "buffer" as the currently lowest
					pipeITookItFrom = i;			//and set which pipe we are using
					bool flag = false;				//not the first time anymore :P
				}
				else if (curLowest.compare(ChildBufs[i]) > 0) {	//Otherwise, check if the next value is lower
					curLowest = ChildBufs[i];		//If it is, it is now the currlowest
					pipeITookItFrom = i;			//Also set which pipe we took it from
				}
			}
			//Refill the buffer which we took it from, for next time.
			//Would prefer to do this asyncrounsly with a thread and mutexes :P
			//will not work 100% yet.  needs to check for null
			ChildBufs[pipeITookItFrom] = getMergeBuff(pipeITookItFrom, procOutFile[pipeITookItFrom]);

			//output that lowest (smallest) string
			cout << curLowest << endl;

			//easy to do the extra credit.
			//instead of just outputting it, store it in a buffer
			//and check to see if the next one is same as the previous one, if not, output previous
			//if so, increment number of instances of that word, which will be outputed once we have found all of that string.
			
			//Not closing for debug safety
			//closeFD(procOutFD[i][0]);	//close FD (Child -> Sorter)
			return 0;
			//We won't actually close and return here.
			//But for testing, one iteration is plenty
			break;
		}
		return 0;
		break;
		//end of child code
	}
	default:
		//Parent Code
		//just quit the parent I suppose
		//NOTE: *was* tested without this as well.
		retun 0;
		break;
	}
	//Safety return
	//(and for compiler)
	return 0;
}
//Ends program with error.
bool kill(string message) {
	perror(message.c_str());
	exit(-1);
	return false;
}
//outputs a string to stderror
bool error(string message) {
	perror(message.c_str());
	return false;
}
//unused as of 90% of this project
string readFileLine(FILE *file) {
	//assuming FILE is valid if it gets called into here.
	char *buff = (char*)malloc(PIPE_MAX);

	if(fgets(buff,PIPE_MAX,file) == "eof")
		return std::string("eof");

	string temp = string(buff);
	free(buff);
	return temp;
}
//Opens a FD as a FILE stream object
FILE* openFD(int fd, string mode) {
	FILE* file = NULL;
	file = fdopen(fd, mode.c_str());
	if (file == NULL)
		kill("Error opening FD as a file");
	return file;
}
//Closes a FILE stream object
void closeFile(FILE* fileToClose) {
	//This debug code left here on purpose
	cout << "\nClosing File: " << fileToClose;
	if (fclose(fileToClose) == -1)
		kill("Error closing file");
}
//Close a FD
void closeFD(int fd) {
	if (close(fd) == -1)
		kill("Error closing file descriptor");
}
//wait / waitpid
//convert a NULL TERMINATED c_string to a C++ string
string charToString(char inArray[]) {
	string final = "";
	for(int i = 0; i < PIPE_MAX; i++) {
		if (inArray[i] == '\0')
			return final;
		else
			final += inArray[i];
	}
	return final;
}
//Refin the specified Merge buffer
string getMergeBuff(int whichBuff, FILE* whatFile) {
	char tempWord[100];
	if (fgets(tempWord,PIPE_MAX,whatFile) == NULL)
		return NULL;
	return charToString(tempWord);
}