CXX = icpc
CXXFLAGS = -Wall -openmp -O3 -g
#overkill on the flags, but that doesn't hurt anything.
LDFLAGS = -lrt -lpthread
#replace this if you want to change the output name.
TARGET = uniqify

#any headers go here
INCLUDES = stdinc.h

#any .c or .cpp files go here
SOURCE = uniqify.cpp

#default is to compile
default: compile

#depends on all of you source and header files
compile: ${SOURCE} ${INCLUDES}
#this assumes you actually are linking all of the source files together
	${CXX} ${CXXFLAGS} ${SOURCE} -o ${TARGET} ${LDFLAGS}

debug: ${SOURCE} ${INCLUDES}
	${CXX} ${CXXFLAGS} ${SOURCE} -o ${TARGET} ${LDFLAGS} -DDEBUG

clean:
	rm uniq uniqify -f