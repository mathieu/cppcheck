SRCS=CheckHeaders.cpp  CommonCheck.cpp  tokenize.cpp
OBJS=$(SRCS:%.cpp=%.o)

%.o:	%.cpp
	g++ -Wall -pedantic -g -I. -o $@ -c $^

all:	${OBJS} main.o
	g++ -Wall -g -o cppcheck $^

clean:
	rm -f *.o cppcheck_test cppcheck
