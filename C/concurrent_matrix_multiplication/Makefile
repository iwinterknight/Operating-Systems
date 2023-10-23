CC=gcc
LD=ld
WARNS=-Wall -pedantic -Wextra
CFLAGS=-g3 -std=gnu11 ${WARNS}
LIBS=-lpthread -lrt

all: tags matmul

tags: *.c
	#ctags -R .
	#etags -R .

matmul: matmul.o
	${CC} -g -o $@ $^ ${LIBS}

matmul.o: matmul.c
	${CC} -g -c $<

%.o:%.c *.h
	${CC} ${CFLAGS} -c $< -o $@

clean: 
	$(RM) matmul shell *.o *~
