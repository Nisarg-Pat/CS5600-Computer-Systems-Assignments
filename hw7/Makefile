all: parent-child-tasks two-threads-and-mutex reader-writer


# We include -lpthread here, for a program that uses POSIX threads.
# Newer versions of gcc allow you to simply invoke '-pthread' instead
#   of '-lpthread'
parent-child-tasks: parent-child-tasks.c
	gcc -g3 -O0 -o parent-child-tasks parent-child-tasks.c -pthread

two-threads-and-mutex: two-threads-and-mutex.c
	gcc -g3 -O0 -o two-threads-and-mutex two-threads-and-mutex.c -pthread

reader-writer: reader-writer.c
	gcc -g3 -O0 -o reader-writer reader-writer.c -pthread

clean:
	rm -f ${FILE} a.out *~

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
