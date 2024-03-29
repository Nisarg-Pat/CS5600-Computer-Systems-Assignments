# You may decide to replace 'gcc' by 'clang' if you have 'clang on your
# local machine.  It has better error reporting.

# Some reasonable tests to try this out might be:
# make all
# ./test1 primtes-test 12345678912345
# ./test1 counting-test 12345678912345

CC=gcc
CFLAGS=-g3 -O0

hw1: ckpt readckpt

check: ckpt readckpt primes-test
	rm -f myckpt  # Remove any older 'myckpt' files
	./ckpt ./primes-test 11 &  # Start the target program in background
	sleep 2
	echo Checkpointing ...
	kill -12 `pgrep -n primes-test`
	sleep 2
	./readckpt myckpt

all: primes-test counting-test test1 test2 test3 test4 test5 \
	proc-self-maps save-restore-memory \
	ckpt readckpt

#========================

# https://stackoverflow.com/questions/36692315/what-exactly-does-rdynamic-do-and-when-exactly-is-it-needed
primes-test: primes-test.c
	${CC} ${CFLAGS} -rdynamic -o $@ $<

counting-test: counting-test.c
	${CC} ${CFLAGS} -rdynamic -o $@ $<

test: test.c
	${CC} ${CFLAGS} -o $@ $<

test%: test libconstructor%.so
	cp $< $@

ckpt: ckpt.c libckpt.so
	${CC} ${CFLAGS} -o $@ $<

libckpt.so: libckpt.o
	${CC} ${CFLAGS} -shared -fPIC -o $@ $<

libckpt.o: libckpt.c
	${CC} ${CFLAGS} -fPIC -c $<

readckpt: readckpt.c
	${CC} ${CFLAGS} -o $@ $<

# -fPIC required for shared libraries (.so)
libconstructor%.so: constructor%.o
	${CC} ${CFLAGS} -shared -fPIC -o $@ $<
constructor%.o: constructor%.c
	${CC} ${CFLAGS} -fPIC -c $<

#========================
proc-self-maps: proc-self-maps.c
	${CC} ${CFLAGS} -DSTANDALONE -o $@ $<

#========================
save-restore-memory: save-restore-memory.c
	${CC} ${CFLAGS} -o $@ $<

#========================

clean:
	rm -f a.out primes-test counting-test
	rm -f libconstructor?.so constructor?.o test? test
	rm -f proc-self-maps save-restore-memory save-restore.dat
	rm -f libckpt.so libckpt.o ckpt readckpt myckpt

dist: clean
	dir=`basename $$PWD` && cd .. && tar czvf $$dir.tar.gz ./$$dir
	dir=`basename $$PWD` && ls -l ../$$dir.tar.gz

.PRECIOUS: libconstructor%.so constructor%.o
