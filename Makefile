all: helper.c helper.h main.c
	gcc helper.c main.c -lreadline -o skish
wc: wc.c
	gcc wc.c -o wc
cat: cat.c
	gcc cat.c -o cat
echo: echo.c
	gcc echo.c -o echo
mkdir: mkdir.c
	gcc mkdir.c -o mkdir
ls: ls.c
	gcc ls.c -o ls
