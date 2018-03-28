CC=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic -D_GNU_SOURCE -Wno-format -Wno-long-long -I.

LDFLAGS=

MAKEDEPEND=${CC} -MM
PROGRAM=test_buffer_pool

OBJS = test_buffer_pool.o

DEPS:= ${OBJS:%.o=%.d}

all: $(PROGRAM)

${PROGRAM}: ${OBJS}
	${CC} ${LDFLAGS} ${OBJS} ${LIBS} -o $@

clean:
	rm -f ${PROGRAM} ${OBJS} ${DEPS}

${OBJS} ${DEPS} ${PROGRAM} : Makefile

.PHONY : all clean

%.d : %.cpp
	${MAKEDEPEND} ${CXXFLAGS} $< -MT ${@:%.d=%.o} > $@

%.o : %.cpp
	${CC} ${CXXFLAGS} -c -o $@ $<

-include ${DEPS}
