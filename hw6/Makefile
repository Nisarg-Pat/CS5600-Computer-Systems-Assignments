FILE=producer-consumer

run: ${FILE}
	./${FILE}

all: ${FILE} temp temp2

check: build
	./${FILE}

build: ${FILE}

# We include -lpthread here, for a program that uses POSIX threads.
# Newer versions of gcc allow you to simply invoke '-pthread' instead
#   of '-lpthread'
${FILE}: ${FILE}.c
	gcc -g3 -O0 -o ${FILE} semaphore.c ${FILE}.c -pthread

temp: producer-consumer-temp.c
	gcc -g3 -O0 -o producer-consumer-temp semaphore-temp.c producer-consumer-temp.c -pthread

temp2: producer-consumer-temp2.c
	gcc -g3 -O0 -o producer-consumer-temp2 semaphore-temp.c producer-consumer-temp2.c -pthread

gdb: ${FILE}
	gdb ${FILE}

vi: ${FILE}.c
	vi ${FILE}.c

emacs: ${FILE}.c
	emacs ${FILE}.c

clean:
	rm -f ${FILE} a.out *~

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
