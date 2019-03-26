makefil: parse.c buitin.c exec.c pipe.c execute.c
	gcc -o shell buitin.c parse.c exec.c pipe.c execute.c -I. 
