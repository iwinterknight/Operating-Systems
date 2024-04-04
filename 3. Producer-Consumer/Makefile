CC=gcc
LD=ld
WARNS=-Wall -pedantic -Wextra
CFLAGS=-g3 -std=gnu11 ${WARNS}
LIBS=-lpthread -lrt

all: tags producer_consumer

tags: *.c
	#ctags -R .
	#etags -R .

producer_consumer: producer_consumer.o
	${CC} -g -o $@ $^ ${LIBS}

producer_consumer.o: producer_consumer.c
	${CC} -g -c $<

%.o:%.c *.h
	${CC} ${CFLAGS} -c $< -o $@


clean: 
	$(RM) producer_consumer shell *.o *~
