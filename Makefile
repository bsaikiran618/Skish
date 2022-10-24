all: skish wc cat echo mkdir ls ps

skish: helper.c main.c helper.h
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
ps: ps.c
	gcc ps.c -o ps
