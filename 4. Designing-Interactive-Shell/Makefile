CC=gcc
LD=ld
WARNS=-Wall -pedantic -Wextra
CFLAGS=-g3 -std=gnu99 ${WARNS}
LIBS=

all: tags mysh

tags: *.c
	#ctags -R .
	#etags -R .


mysh: shell.o
	${CC} -g -o $@ $^ ${LIBS}

shell.o: shell.c
	${CC} -g -c $<

%.o:%.c *.h
	${CC} ${CFLAGS} -c $< -o $@

clean: 
	$(RM) mysh shell *.o *~
