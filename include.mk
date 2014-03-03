OPT=-O3 -funroll-loops -g -DNDEBUG
#OPT=-O0 -g -DMCBSP_SHOW_PINNING

#GNU Compiler Collection:
CFLAGS=-ansi -Wall -std=c99 ${OPT} -I./multicorebsp -L./multicorebsp
CC=gcc
CPPFLAGS=-ansi -Wall -std=c++98 ${OPT} -I./multicorebsp -L./multicorebsp
CPP=g++
AR=ar

#Intel C++ Compiler:
#CFLAGS=-Wall -std=c99 ${OPT} -I.
#CC=icc
#CPPFLAGS=-ansi -Wall ${OPT} -I.
#CPP=icc -diag-disable 378
#AR=ar

%.o: %.c %.h
	${CC} ${CFLAGS} -c -o $@ $(^:%.h=)

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $^

%.opp: %.cpp %.hpp
	${CPP} ${CPPFLAGS} -c -o $@ $(^:%.hpp=)

%.opp: %.cpp
	${CPP} ${CPPFLAGS} -c -o $@ $^

