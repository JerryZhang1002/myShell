FLAGS=-pedantic -std=gnu++98 -Wall -Werror -ggdb3
myShell: parse.o findProgram.o parseinfo.o stack.o myShell.o
	g++ $(CFLAGS) -o myShell parse.o findProgram.o parseinfo.o stack.o myShell.o
myShell.o: myShell.cpp
	g++ $(CFLAGS) -c myShell.cpp
parse.o: parse.cpp
	g++ $(CFLAGS) -c parse.cpp
findProgram.o: findProgram.cpp
	g++ $(CFLAGS) -c findProgram.cpp
parseinfo.o: parseinfo.cpp
	g++ $(CFLAGS) -c parseinfo.cpp
stack.o: stack.cpp
	g++ $(CFLAGS) -c stack.cpp
clean:
	rm -f *.o
