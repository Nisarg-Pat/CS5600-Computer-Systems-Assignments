# First target is default target, if you just type:  make

FILE=hw3.c

CC=gcc
CFLAGS=-g3 -O0

hw3: myshell

build: all

check: myshell
	./myshell

all: myshell pipe-example

default: run

myshell: myshell.c
	${CC} ${CFLAGS} -o $@ $<

pipe-example: pipe-example.c
	${CC} ${CFLAGS} -o $@ $<

run: myshell
	./myshell

gdb: myshell
	gdb --args myshell

emacs: ${FILE}
	emacs ${FILE}

vi: ${FILE}
	vi ${FILE}

clean:
	rm -f myshell pipe-example a.out *~

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.# The shell will view $dir as a shell variable and expand it.
dist: clean
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
