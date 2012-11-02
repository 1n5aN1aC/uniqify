// stdinc.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
//Only define these the first time
#define _BSD_SOURCE

#define HELP "Correct Program Usage:\nuniqify [<number of threads >]\nNumber of threads will be asumed to be 10 if not specified\nYou must pipe or redirect the input into uniqify\n"
#define PIPE_MAX 4096
#define STDIN 0
#define delimiters " 1234567890!@#$%^&*()`~-_=+[{]}|:;'/?.>,<\\\"\n\t\r"

#pragma once

// Reference additional headers your program requires here
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <deque>
#include <list>
#include <queue>