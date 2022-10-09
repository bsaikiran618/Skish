all: helper.c helper.h main.c
	gcc helper.c main.c -lreadline -o skish
