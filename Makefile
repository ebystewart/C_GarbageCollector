CC=gcc
CFLAGS=-g
LIBS=
OBJS= gcol.o	\
	test.o	

GarbageCollector.bin:${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o GarbageCollector.bin

gcol.o:gcol.c
	${CC} ${CFLAGS} -c gcol.c -I . -o gcol.o

test.o:test.c
	${CC} ${CFLAGS} -c test.c -I . -o test.o


all:
	make

clean:
	rm *.o
	rm *.bin