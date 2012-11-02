// myar.cpp : main project file.

#include "stdinc.h"

using namespace std;

#define HELP "Correct Program Usage:\nuniqify [<number of threads >]\nNumber of threads will be asumed to be 10 if not specified\nYou must pipe or redirect the input into uniqify\n"

int workers = 10;

int main(int argc, char** argv)
{
	if (argc < 2) {
		cout << Assuming 10 Working Processess << endl;
		workers = 10;
	}
	else if (argc > 3)
		cout << HELP << endl << "I can't handle all this awesomness!\n" << "Too many arguments!" << endl;
	else
		workers = atoi (argv[1]);
	
	cout << workers;
	
	bool quit = false;
	while (!quit) {
		if (
		
		
		
		
	}	
	
	return 0;
}

readLineFromCIN() {
	cout << "derp";
	string bufferIn = readFromLine(file, PIPE_MAX);
	boost::split(splitAr,newBufferString,boost::is_any_of(delimiters));
}