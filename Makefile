all: helper.o skish

helper.o: helper.c helper.h
	gcc helper.c -c
skish: main.c *.o
	gcc main.c *.o -o skish
