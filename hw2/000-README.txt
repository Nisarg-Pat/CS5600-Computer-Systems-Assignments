                        Homework 2 (HW1, continued)
                      OBSOLETE: DUE:  Tuesday, Sept. 27
                      DUE:  Friday, Sept. 30
                      [ hw2:  Please include all of Sections A, B, C ]

Copyright (c) Gene Cooperman, 2021
This may be copied, modified, and re-distributed (and additional
copyrights added) as long as this copyright notice remains.

========================================================================
You can modify what you submitted for hw1, and then add Part C, for restart.
Since hw2 includes hw1, if the grade for hw2 is higher than hw1, then
I will raise the grade for hw1 to be the same as hw2.
========================================================================

========================================================================
FORMAT FOR SUBMISSION:
As before, you will use 'make dist' to create a .tar.gz file to submit.
    Now that you all know about Makefile, please be sure to include
the targets 'build', 'check', and 'clean' in your targets.  The 'build'
should compile all your source code.  The 'check' should run a test.
A nice recipe for 'check' might be (remember the '<TAB>'):
check: build
	./ckpt ./counting-test 17 &
	sleep 4
	kill -12 `pgrep --newest counting-test`
	./restart myckpt
And 'make clean' should use 'rm -f ...' to remove any files that were
created by 'make build' or 'make check'.
    Your code will be tested on Khoury:  ssh login.ccs.neu.edu
Please verify that your code works on Khoury login.

    And please also include a 'README' file for the grader saying what works,
and what doesn't work.  (Ideally, you'll just write that it all works. :-) )
========================================================================

====
MOTIVATION AND RATIONALE FOR THIS ASSIGNMENT:

In this assignment, you will build a mini-ckpt program.
The software that you produce will be self-contained.

The rationale for this assignment is that it provides a rapid overview
of concepts that will be studied in greater depth later in the course.
The work will be done in a supportive environment, allowing unlimited
resubmissions until the concepts are clear to each student.  This
assignment represents a valuable _real-world_ program that you may wish to
use in your future work.  In principle, this assignment is self-contained,
and should require almost _nothing_ beyond the prerequisites for this
course, although it requires applying the concepts in a new manner.

The only system calls that introduce truly new concepts for most
students are 'mmap' and 'munmap' and 'getcontext' and 'setcontext'.
We will also use the techniques of LD_PRELOAD, constructor functions,
and signal handlers.  Beyond that, /proc/self/maps (memory segments)
is used.  Aside from that, this assignment briefly uses some concepts
from an undergraduate systems course that may be new to some students:
assembly; memory layout (text/data/stack, call fames); the system calls
used in a shell (especially, signal (and signal handlers), kill, open,
close, and the calls that were discussed but not used:  "read" and
"write"); standard Linux productivity tools (e.g., Makefile/make, GDB,
and pointers (especially the operators: "*", "&", and the additional
operator "sizeof", and data structures in C.  These concepts from an
undergraduate course will be rapidly reviewed in the course lectures.

====
DESCRIPTION OF ASSIGNMENT:

You will write a dynamically linked library, libckpt.so.  Dynamically
linked libraries are create using:
  gcc -shared -o libckpt.so libckpt.c

The techniques used are mostly covered in the example programs that
are provided here.  See the file BACKGROUND, included here,
and the file Makefile (Do 'make all' to build all the examples.)
The techniques will only support checkpointing single-threaded
programs (not multi-threaded).

(Note that the memory layout of a process is stored in /proc/PID/maps,
 as described in the BACKGROUND file.
 The fields such as rwxp can be read as "read", "write", "execute"
 and "private".  Naturally, rwx refer to the permissions.
 Private refers to private memory.  If you see "s" instead
 of "p", then it is shared memory.)

==================================================================
ORDER OF READING:

Recommended reading order:
  I.   000-OVERVIEW - conceptual overview
  II.  000-README - step-by-step description of what to implement and how
                    [ THIS DOCUMENT; see below ]
  III. BACKGROUND - example programs that illustrate the systems techniques used
However, if something is unclear, feel free to stop in the middle
(for example, in the middle of OVERVIEW), and read an appropriate
section for details (e.g., in 000-README.)

==================================================================
The assignment has three parts:
A. SAVING A CHECKPOINT IMAGE FILE
B. READING THE CHECKPOINT IMAGE FILE AND PRINTING THE INFORMATION
C. RESTARTING FROM A CHECKPOINT IMAGE FILE
D. WHAT TO HAND IN

APPENDICES:
E. SIMPLE TIPS FOR DEBUGGING
F. ADVANCED TIPS FOR DEBUGGING (probably not needed for most students)
G. ADVANCED TOPIC:  HOW TO PORT THIS FOR RECENT Linux DISTROs (kernels)

==================================================================
GOAL:  ./ckpt ./a.out
  (save the state of a.out in a file, 'ckpt' (sometimes called 'myckpt'
   in class lectures))

A. SAVING A CHECKPOINT IMAGE FILE:
0.  Write ckpt.c and compile to ./ckpt
     a. setenv("LD_PRELOAD", "/.../libckpt.so", 1);
     b. execvp(argv[0], &argv[1]);
     [ This is the main idea.  Please check the exact syntax.
       For the mysterious '&argv[1]', note that in C, an array is
       the same thing as a point to the first element of the array.
       So, for example, in:  ./ckpt test-primes 17
       the 'main' routine of 'ckpt' will see:
         { argv: "./ckpt", "test-primes", "17", NULL}
       So, &argv[1] will be:
         { "./ckpt", "test-primes", "17", NULL}
     ]
1.  You will preload your libckpt.so when the target executable is run.
     For an example of this, see constructor1.c, used as part of test1,
     as mentioned in BACKGROUND and in Makefile.  The rest of this
     section describes how to build a C file, libckpt.c, that is compiled
     to libckpt.so.
2.  You will need a simple way to trigger the checkpoint.  In libckpt.c,
     write a signal handler for SIGUSR2, that will initiate all
     of the previous steps.
     [ To trigger a checkpoint, you now just need to do from a command line:
         kill -12 PID
       where PID is the process id of the target process that you wish
       to checkpoint.  (Signal 12 is SIGUSR2, reserved for end users.)]
     For an example of this, see constructor4.c, used as part of test4,
     as mentioned in BACKGROUND and in Makefile.
     IMPORTANT:  Your constructor function will not include 'exit()'.
       After the constructor returns, the linker will pass control to 'main()'.
       You should _not_ call 'main()' from your constructor.  That trick
       worked in 'constructor2.c' through 'constructor5.c' _only_
       because we compiled 'primes-test.c' and 'counting-test.c'
       using 'gcc -rdynamic'.  But normal end user code would not be
       compiled with the '-rdynamic' flag.
     HINT:  If you are testing while running under GDB, and a SIGUSR2
       arrives, then you can use '(gdb) signal SIGUSR2' to pass the
       signal from GDB to your target application.
3.  The signal handler will only help you if you can call 'signal()'
     to register the signal handler.  The call to 'signal()' must
     occur even before 'main()' is called in the user's executable.
     It is not enough that libckpt.c is part of libckpt.so, which is
     preloaded.  We also need to "pre-execute" the call to 'signal()'
     To do this, we will define a constructor function inside libckpt.c
     (inside libckpt.so).  If you have ever seen or written code that
     initializes a _global_ variable, myObj:  Obj myObj = new Obj();
     then you have seen a constructor in an object-oriented language
     like C++ or Java.  In C, we create a constructor function using
     the syntax shown in constructor1.c (invoked via: ./test1 EXECUTABLE).
     See BACKGROUND, Makefile, and constructor1.c.
4.  After a SIGUSR2 has signaled a request for a checkpoint, your
     signal handler will need to call a function to read the current
     memory segment from /proc/self/maps (which is the memory layout
     of the current process.  For an example of this, see proc-self-maps.c,
     mentioned in BACKGROUND and in Makefile.
     NOTE:  You may or many not need to call proc_self_maps() twice,
            because during the first call, it might call mmap() and alter your
            /proc/self/maps.  Try running first with only one call
            to proc_self_maps().
5.  Your signal handler will need to practice saving each memory segment
     in a checkpoint image file.  For an example of this, see
     save-restore-memory.c, mentioned in BACKGROUND and in Makefile.
     That example saves a single function in a file, and then restores
     it at a _new_ location in memory.  Pay special attention to the
     coding of read() and write().  As described in the man pages, read()
     and write() may read/write only part of the full data requested.
     They then return an 'int' for how much of the data was actually read
     or written.  This is the reason that the example code places read()
     and write() in a 'while' loop.  Now write your own function
     that can save an entire memory segment, assuming that the function
     has arguments for the start and end address of the memory segment.
6.  Your signal handler should then save _all_ memory segments in a
     checkpoint image (either through a new function, or as part of the
     function that read /proc/self/maps).  The information should be
     saved in a checkpoint image file.  One possible structure for the
     file would be:  HEADER1, DATA1, HEADER2, DATA2, ....  In this case
     a HEADER might possibly look like:
      struct ckpt_segment {
        void *start_address;
        void *end_address;
        int read, write, execute;
        int is_register_context; // Is this a context from getcontext?
                                 // If so, ignore the start_address,
                                 //   end_address, and read/write/execute.
        int data_size;  // Size of DATA after this HEADER.
        char name[80];  // Optional (used only for debugging)
      }
    This struct is similar to the one found in 'struct proc_maps_line'
    from proc-self-maps.c.  See BACKGROUND and Makefile for more information.
7.  In the last step, we saved all memory segments.  In fact, [vdso] and
     [vsyscall] (and [vvar] on some Linux distros) are special.  They are
     injected by the Linux kernel and specialized for the current process.
     Don't save them in your checkpoint image file.  On restart, we will
     use the new [vdso], [vsyscall] and [vvar] memory segments provided by the
     kernel for the new process.
     *** IMPORTANT:  Try saving the "[vdso]" segment anyway.  If your
     restarted process uses a vdso segment at a different address,
     then you can fix it by restoring the original vdso segment (assuming
     that you are restarting on the same computer where you checkpointed).
    For technical reasons associated with glibc being compiled with
     a "stack protector", you will need to use getcontext/setcontext,
     and _not_ sigsetjmp/siglongjmp.  The file constructor5.c has
     been revised to also provide an implementation using
     getcontext/setcontext.  In addition, please read the file
     save-restore-context.c for example code using getcontext/setcontext
     that can be included in your own code.
8.  When you write to the 'ckpt' file, there is one other issue to
     beware of.  If you examine '/proc/self/maps', you will see that
     there are some memory segments with permission "---p".  Essentially,
     the process does not have permission to read of write to this
     segment.  This was inserted by the compiler/linker as a "guard page",
     to protect against buffer overruns.  If the application has a bug
     that causes it to read beyond some memory segment, there will be
     a "guard page" immediately after it, and the application will stop
     with a segmentation fault, since it cannot read from permissions
     "---p".  Therefore, the application crashes early instead of
     continuing the buffer overrun.  You cannot write from this memory
     segment to your "ckpt" file if the memory has no read permission.
     I suggest simply skipping this memory segment and not saving it.
9.  And there is one more "ckpt_segment" that we will need to save.
     On restart, after we restore all of the memory segments, we will
     need to "jump" back into the original code at the old location
     given by the previous program counter ($pc).  Luckily, Linux has
     system calls for saving and restoring _all_ registers (including
     $pc and $sp).  These are 'getcontext()' and 'setcontext()'.  Make sure
     that your checkpoint image file uses 'getcontext()' to save a copy
     of the checkpoint image file.  For an example of this, see
     constructor5.c, used as part of test5, as mentioned in BACKGROUND
     and in Makefile.  Note that the registers are saved in a data
     structure of type 'ucontext_t'.  So, the 'data_size' field
     of ckpt_segment will be set to 'sizeof(ucontext_t)'.
10.  CONGRATULATIONS!  You've completed half the work.  You now have a
     checkpoint image file.  For consistency, let's call this file 'myckpt'.
     Let's assume that your checkpointing program (analogous to 'text5')
     is called checkpoint.c

==================================================================
B. READING THE CHECKPOINT IMAGE FILE AND PRINTING THE INFORMATION:
1.  Let's do a consistency check, before moving on to the RESTART phase.
     One of the important lessons of computer systems is to always
     take _small_ steps, and then to go back and verify that it works.
     Write a program, readckpt.c, that will read your checkpoint
     image file ('myckpt'), and will print out some information
     about what it contains.  Let's call this new program 'readckpt.c'.
     The program, readckpt.c, should at least print out thee start
     address, end address, and filename (if any) for that segment.
     OBSOLETE: If the DATA in 'myckpt' has 'is_register_context'
               set to 1 (true), then print the $pc.  [ It seems to
               be difficult to find the $pc in the ucontext_t data structure. ]
     Optionally, you may choose to print in hexadecimal a few words
     from ucontext_t, so that you can compare them
     to the restart phase.  It's important to verify that those words
     are non-trivial (non-zero).
2.  Let's compare for consistency.  Run your program:
       ./checkpoint EXECUTABLE
     where ./checkpoint is your version of "test5/constructor5.c"
     in the examples provided, and it is preloading your libckpt.so..
     Make sure that EXECUTABLE runs for a long time (maybe an infinite loop?).
     Look up the pid of EXECUTABLE (let's call that PID).  Then force
     a checkpoint:  kill -USR2 <PID>   [ OR:  kill -12 <PID> ]
     and compare the output of reading the checkpoint file:  ./readckpt myckpt
     with the actual memory segment information.
3.  WHAT TO VERIFY:
     (i)  Do the memory segment addressess agree?  (Try running
          './checkpoint hello', and suspend after the checkpoint, but
          before it exits.  Compare 'cat /proc/PID/maps' to the output of
          './readckpt myckpt'.)
     (ii) Does 'ls -l myckpt' show a file size comparable to the size of all
          the memory segments?  (If not, did you write the data to myckpt?)

=================================================================

C. RESTARTING FROM A CHECKPOINT IMAGE FILE:
1.  Create a file, restart.c.  The critical issue in restarting is
     that our restart process might use some of the same addresses as
     the original process that we are attempting to restore.  So, we
     need to take steps to avoid an address conflict.  We will give
     special instructions to the linker about where to load restart:
      gcc -static \
        -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 \
        -g3 -o restart restart.c
    [ Note:  The gcc syntax '-Wl,Ttext=...' means pass on the parameter
             '-Ttext=...' to the linker that will be called by gcc.
             See 'man ld' for the Linux linker. ]
      Note:  In reading 'man ld', you'll see that numbers like 500000000000
             will be interpreted as hexadecimal, _not_ decimal. ]
     Hopefully, addresses like 0x5000000 were not used by your original
     process.  Furthermore, the '-static' flag says to build this
     using the older statically linked format.  In this format, the text
     segment and data segment of libc.so (actually from libc.a for statically
     linked libraries) will be merged into one single text segment, and
     one single data segment.  Verify this through:
       readelf -S ./restart
     Also, verify that the running process will have this property:
       gdb --args ./restart
       (gdb) break main
       (gdb) run
       (gdb) info proc   # And note the PID of the process
       (gdb) shell cat /proc/PID/maps # ... replacing PID, by its value
     For comparison, do:
       ldd ./checkpoint
       readelf -S ./checkpoint
     and you'll see lots of extra libraries.
2.  We are now guaranteed that the text and data segments of ./restart will
     not have any conflicts.  But we still have to worry about the stack
     segment.  There are two ways to accomplish this.
    (i) In this method, you will grow the stack with many call frames,
        so that the latest call frame will be at an address with no
        conflict.  So, you can write a recursive function:
          void recursive(int levels) {
            if (levels > 0) {
              recursive(level - 1);
            } else {
              do_work();
            }
          }
        You will then call 'recursive(1000)', and define do_work to
        restore the memory from the checkpoint image.  In GDB, do:
          (gdb) break do_work
          (gdb) print $sp
        and verify that the address for $sp does not conflict with
        any of your addresses seen from:  ./readckpt myckpt
    (ii) Alternatively, you can mmap a new memory region at an
         address that doesn't conflict:
           mmap(0x6000000, 0x1000000, PROT_READ | PROT_WRITE,
                MAP_FIXED | MAP_ANONYMOUS, -1, 0);
         IMPORTANT:  Then set the stack pointer to something like
                     (0x6000000 + 0x1000000 - 16), which is near the
                     highest address, since the stack will grow _downward_.
         You will set the $sp register using inline assembly language
         inside your C program.
           void *stack_ptr = 0x6000000 + 0x1000000 - 16,
           asm volatile ("mov %0,%%rsp;" : : "g" (stack_ptr) : "memory");
           do_work();
         This will start a completely new stack and a new call frame.
    NOTE:Later, we will do 'setcontext()', which will replace '$sp'
         once again, but this time pointing to the original '$sp'
         at the time of checkpoint.
3.  You are now ready to map in the memory segments from the checkpoint
      image file.  (See save-restore-memory.c for an example.)  We will
      map in each memory segment in 'myckpt'.  We will use the MAP_FIXED
      flag from 'man mmap' to specify the exact address to restore the segment.
      There should be no address conflict with the text and data segments
      of 'restart'.  However, there could still be a conflict with the old
      stack of the 'restart' process.  Luckily, 'man mmap' tells us that:
        "If the memory region  specified  by addr and len overlaps pages
         of any existing mapping(s), then the overlapped part  of  the
         existing mapping(s)  will  be discarded." [See MAP_FIXED section]
      Add code for this to do_work().  As a precaution, it is good
      practice to check if the 'mmap' failed:
        void *addr = mmap(...);
        if (addr == MAP_FAILED) { perror("mmap"); }
      [ While you should not need this, note that the 'mprotect' syscall
        is also available, if you wish to change the permissions of
        an existing memory segment. ]
4.  You are now ready to restore the original registers saved in 'myckpt'.
      Read 'man getcontext' and 'man setcontext' again.  Also, create
      a variable:
        ucontext_t old_registers;
      To read the ucontext_t from 'myckpt' into old_registers:
        read(fd, &old_registers, sizeof(ucontext_t);
      When you were reading each memory segment from 'myckpt', you
      should have specially recognized the field 'is_register_context'
      of 'struct ckpt_segment'.  When 'is_register_context' was 1 (true),
      you should have executed the 'read()' statement above to read
      the ucontext_t from 'myckpt'.  Add that code to restart.c now.
5.  You have now restored all of the memory segments from your checkpoint
     image, and you have a ucontext_t ready.  You are now ready to
     "pull the trigger".  Add to your code in restart.c:
       setcontext(old_registers);
     (And read 'man setcontext' for any include files that you'll need.)
     IMPORTANT:  You will call (enter) the setcontext, but because all
                 previous registers ($pc, $sp, $v0, etc.) will be restored,
                 you will actually return from getcontext.  So, you now have
                 two possible ways to return from getcontext (during ckpt and
                 during restart).  So, you need to go back to your code
                 in the signal handler function, and modify it.
                 See the use of 'is_restart' in save-restore-context.c
                 as one example of how to distinguish the two cases.
                 (See 000-OVERVIEW for another view of this situation.)
6.  If anything goes wrong (like a segfault), you will want to debug
     at the assembly level:
       gdb --args restart myckpt
       (gdb) break main
       (gdb) run
       (gdb) break LINE  # where LINE is the line at which you call setcontext
       (gdb) x/20i $pc
       (gdb) si  # and repeat.  'si' is "step into (assembly) instruction"
       (gdb) ni  # and 'ni' [next (assembly) instruction] is then useful
                 # to skip past a helper function (skip past a 'callq').
       (gdb) # When it segfaults, print $pc and other registers.  Compare with
       (gdb) #  the intended target $pc (printed by your 'readckpt' program).
    To discover the intended target $pc, you can run
      ./checkpoint EXECUTABLE
    under GDB, and set a breakpoint at your signal handler function before
    executing 'run'.  Then, when you checkpoint, just after calling
      getcontext(...)
    do:  (gdb) print $pc
         [ You can also use GDB to print the internal data of your ucontext_t.
    Now you have a known target $pc in the 'myckpt' that you generate.
    When you restart, during assembly-level debugging, see if setcontext()
    is taking you to that address, and if that address corresponds to
    the correct memory segment.

7.  Here are some other things to check if 'setcontext' is not working
    for you.  In all cases, do 'gdb --args ./restart myckpt' and then
    do '(gdb) break setcontext' and '(gdb) run'.  You are then about
    to do 'setcontext'.
    (a)  '(gdb) frame 1' ; '(gdb) print/x context'
    (b)  Compare your /proc/PID/maps with what you see in:
         (gdb) shell ./readckpt myckpt
         Are any segments missing?  Do some segments have the wrong permission?
    (c)  If you continue, does it print "RESTARTING" or "WRITING CKPT"
         (Hopefully, in your signal handler, after 'getcontext', you
         should have printed one or the other based on the value
         of 'is_restart', where 'is_restart' is based on the usage
         in save-restore-context.c .)
    (d)  If you don't see "RESTARTING" or "WRITING CKPT", then
           maybe you made a system call, and didn't recognize the error return.
         Returning with an error, and continuing to execute later lines
           of code makes a bug _really_ hard to find.
         For any non-obvious system calls, _always_ use perror:
           void *rc = mmap(addr, length, prot, flags, fd, offset);
           if (rc == MAP_FAILED) { perror("mmap"); exit(1); }
         OR:
           int rc = open(pathname, O_RDONLY(;
           if (rc == -1) { perror("open"); exit(1); }
    (e)  Try doing 'si' (or 'si 8' for 8 at a time.  Continue until
         the address of your current assembly instruction changes by a lot.
         That should mean that you are now in the text segment of
         libckpt.so.  Verify this by comparing your current address with
         the text segment of libckpt.so, from './readckpt myckpt'.
         (I hope that you chose to print filenames in your 'readckpt',
         since it will make it easier to manually read each line.)
    (f)  If you fail before then, typically failing on the  'fldenv'
         assembly instruction, it may mean that something is wrong with
         your variable 'context'.  When you called 'setcontext(&context)',
         'context' was placed on the stack, and fldenv is now trying to
         access that value.
    (g)  If you succeed in reaching libckpt.so in 'hello', and it
         says "WRITING CKPT" instead of "RESTARTING", then review whether
         you had set 'is_restart' to 1 before writing your data segments
         to 'myckpt' during checkpoint.
    (h)  If you are in 'hello' and it says "RESTARTING", then you're most
         of the way there.  it's also good to do a sanity
         test on the original stack from 'hello'.  In GDB, do:
         (gdb) x/100 $sp
         [ Make sure that the call frames on your stack are not mostly zero. ]
         Another possibility is that you may need to copy or mremap
         the current location of "vdso" to the original location
         of "vdso" prior to checkpoint.
         (See 'man mremap' and in order to use this, you'll need to
         add to your checkpoint image file a header for the name "[vdso]"
         with its start address.  Since you're probably staying on the
         same computer, it's safe to just restore "vdso".  But if you
         want this to work on a different computer with a different
         Linux kernel, then using "mremap" may be safer.)

    (i) IMPORTANT:  On restart, your stack looks like it did prior
        to _saving your proc-maps_ to 'myckpt'.  Any call frames from
        function calls that were made _after_ saving to 'myckpt' wil
        be _lost_ on restart.
        DIAGNOSIS:  Look up the function, foo, that calls 'getcontext'.
          In the outer function that calls foo, try to print anything
          after calling 'foo'.  That will fail if your call frame is not right.
        FIXING THE PROBLEM:  After saving 'proc-maps' to 'myckpt',
          call 'getcontext' in that _same_ call frame.
        DO NOT call a new, inner function and then call 'getcontext'
          from the inner function.  When restarting,
          the "current call frame" on the stack will be different
          from what you think.
        DO NOT save your proc-maps in an inner function.  When restarting,
          the "current call frame" on the stack will be different
          from what you think.

    (j) NOT REQUIRED:
          In this simplified form of checkpoint-restart, it will fail
          after restart, when the 'main' function reaches the end.
        DIAGNOSIS:  It continues to print on restart, but then fails
        inside the system call 'exit()', called from 'main'.
        UNDERSTANDING THE DIAGNOSIS:  If you were to look at the assembly
        code at that point, you could discover the reason.
        NOT REQUIRED:
          Try:   
          (gdb) break __run_exit_handlers+222
          (gdb) display/10i $pc
          (gdb) si  # and repeat
        You'll then see that it segfaults at:  xor    %fs:0x30,%rax
        in GDB:
          0x7fe69ff2acee <__run_exit_handlers+222>:    xchg   %ax,%ax
          0x7fe69ff2acf0 <__run_exit_handlers+224>:    mov    0x8(%rcx),%rax
          0x7fe69ff2acf4 <__run_exit_handlers+228>:    ror    $0x11,%rax
          0x7fe69ff2acf8 <__run_exit_handlers+232>:    xor    %fs:0x30,%rax
          0x7fe69ff2ad01 <__run_exit_handlers+241>:    callq  *%rax 
        The '%fs' register in x86 assembly is used for thread-local
        variables.  This is an advanced topic.  If you want to handle the
        case of thread-local variables, then you must examine:
          'man arch_prctl'
        and use ARCH_GET_FS and ARCH_SET_FS .
        NOTE:  'man arch_prctl' is horribly ambiguous.  The correct syntax is:
          unsigned long int fsBase;
          arch_prctl(ARCH_GET_FS, &fsBase);
          arch_prctl(ARCH_SET_FS, fsBase);
          i.  And don't forget the '#include' from 'man arch_prctl'.
          ii. And make sure to call ARCH_GET_FS _before_ saving
              your memory to 'myckpt'.  Otherwise, you won't see that
              data again, during restart.
        NOTE:  One could save fsBase and restore 'myckpt'.  But in this case,
          you can alternatively save fsBase as a local variable inside your
          signal handler.  After you restore all memory segments and
          return from 'setcontext', you will find 'fsBase' in your
          local call frame exactly as you left it at the time of checkpoint.

    (k) It is _NOT REQUIRED_ to make this work outside of Khoury
        login.ccs.neu.edu .  However, if you wish to make it work
        on Windows WSL (or any Linux with a more recent compiler),
        then you may encounter:
          % ./restart myckpt
          *** stack smashing detected ***: terminated  
        DIAGNOSIS:  In your libckpt.c, try adding a loop, 'while (1);'
        after returning from 'getcontext' during restart.  It will hang
        there, but not crash with the "stack smashing" message.
        (Optionally, include:
         'printf("IN ORIGINAL PROGRAM\n"); fflush(stdout);'
         before the 'while (1);' statement.
        UNDERSTANDING THE DIAGNOSIS:
        This is because newer compilers add a cyber-security "feature" to
        detect and prevent "buffer overruns".  The feature is that at
        the beginning of a new function call, they generate a random
        integer in the call frame of that function (on the stack).
        When they return from the function, they verify that they are
        seeing the same random integer, or else esit with the message
        "stack smashing detected".  Since "./restart" is run in a
        different process, it expects a different random integer.
        (This random integer is sometimes colloquially called a "canary",
        as in "the canary in the coal mine".)
            You can turn off this security "feature" by compiling your
        libckpt.c with 'gcc -fno-stack-protector'.

8.  CONGRATULATIONS!  You now have checkpoint-restart working.
    But can we support checkpoint-restart-checkpoint?
    After restarting, can we checkpoint again?
    OPTIONAL ENHANCEMENT:
        If you test, you'll find that trying to checkpoint again
    after a restart will not work.  The reason is that we registered our
    SIGUSR2 signal handler with the kernel originally in the constructor
    function, when launching 'hello'.  But now we are in a re-mapped
    restart process, that the kernel thinks is different from the original
    'hello' process.  So, we need to register our signal handler one
    more time, for the 'restart' process.  A good place to add another
    'signal(SIGUSR2, signal_handler)' is in the existing signal handler,
    just after we return from 'getcontext()'o    during the restart phase.

==================================================================
D. WHAT TO HAND IN:
[ For hw1, you will only hand in files related to:
    checkpoint and readckpt
  For hw2, you will hand in all files below. ]

You must provide a Makefile for this project.  Please have the first
target of your Makefile make your three executables.
For example, you could add the target 'hw5' even before 'all', as follows::

hw5: checkpoint restart readckpt

checkpoint: libckpt.so
	... # FILL IN

# And add targets for restart, readckpt, libckpt.so, etc.

check: ... # See below for a suggested 'check' target.

This assumes that your Makefile has targets for 'checkpoint' and 'restart'
later in the Makefile.  Then 'make' will make the first target ('hw5'),
which will make your 'checkpoint' and 'restart' executables.

In addition, you should include a target 'check'.
This will enable the grader to simply do 'make check' after untar-ing
your files.
Here is a suggest 'check' target (assuming that you are testing
using 'hello'), but you may customize it for yur own filenames, etc.

# This assumes that your application writes the checkpoint into 'myckpt'.
# 'kill -12' sends a
check: checkpoint restart hello
	rm -f myckpt  # Remove any older 'myckpt' files
	./checkpoint ./hello &  # Start the target program in background
	sleep 2
	echo Checkpointing ...
	kill -12 `pgrep -n hello`  # Send a SIGUSR2 to the pid of 'hello'
	sleep 2
	echo Restarting ...
	pkill -9 hello  # Kill your 'hello' program
	./restart myckpt

Assuming that all of your documents are in a directory 'mycheckpoint'
(or possibly a subdirectory of that), you can create a tar file to
submit by simply executing:
  make dist
If your homework is in a directory, mycheckpoint, then 'make dist' will create:
  .../mycheckpoint.tar.gz   [ or ../hw1.tar.gz if you use that directory 
Then use the usual submit script to submit.  That will be as simple as:
  /course/cs5600/homework/submit-cs5600-hw1 mycheckpoint.tar.gz

==================
APPENDIX: E. SIMPLE TIPS FOR DEBUGGING

As discussed above, a nice technique for checking your memory segments is:
(gdb) info proc
(gdb) shell cat /proc/XXX/maps

This will allow you to verify that your /proc/XXX/maps is the same _before_
(during checkpointing), in 'myckpt',  and _after_ (during restart).
Execute 'shell cat /proc/XXX/maps' in 'checkpoint' and in 'restart'.
Also, look at the output of your 'myckpt':
  readckpt myckpt
and verify that it agrees with what you saw when running 'checkpoint'.

When you are debugging "restart", you will often want to verify
that the data that you read back from "myckpt" is the same as the
data that you thought you wrote into "myckpt" during checkpointing.
This is easy to do within GDB.
(a) WITHIN ./hello (used for checkpointing):
  (gdb) print env
    [ ASSUMING THAT YOUR CODE USES:  sigjmp_buf env; AS IN constructor5.c
      YOU MAY HAVE USED 'old_registers' AS IN SECTION C (RESTARTING FROM ...).]
(b) WITHIN ./restart (used for restarting):
  [ AFTER READING BACK env FROM the myckpt FILE. ]
  (gdb) print env
    [ ASSUMING THAT YOUR CODE USES:  sigjmp_buf env; ]
(c) THEN visually compare the "before" and "after" to make sure they
    are the same.

You can also verify that your /proc/XXX/maps is the same _before_
(during checkpointing) and _after_ (during restart).
In "restart" (under GDB), finish calling "mmap" and "read" to
read in all the memory regions from your "myckpt" file.
Then do (as above):
(gdb) info proc
(gdb) shell cat /proc/XXX/maps
    Finally, compare what you see with what you see if you do the
same thing inside your ./hello program during checkpoint.  Verify, visually,
that the memory regions correspond to the same addresses.

For ease of use, note that GDB has a command, signal:
(gdb) run  # And use ^C to interrupt, once we're in the target program.
(gdb) break my_signal_handler
(gdb) signal SIGUSR2
The result is _exactly_ the same as if you had done 'kill -USR2 <PID>'
from a separate terminal.

If your code is failing and exiting in GDB, do:
(gdb) break _exit
(gdb) run  # It will stop at _exit()
(gdb) where

If a system call has returned an error, and you want to know the 'errno', try:
(gdb) pring perror("")

==================
APPENDIX: F. ADVANCED TIPS FOR DEBUGGING

Most students will not need these tips.  During 'restart', the two most
subtle tasks to accomplish are the use of 'mmap' to restore the memory
segments from 'myckpt', and to use 'setcontext' to restore the correct
$pc and $sp.  Usually, you'll be able to use GDB when restoring with 'mmap'.
So, frequent use of '(gdb) shell cat /proc/XXX/maps' should help you there.

This leaves the challenge of debugging 'setcontext'  Hopefully, that
just works for you.  But 'setcontext' will change $pc itself, and will
then land inside the old 'setjmp' function that was invoked within
'checkpoint'.  This will probably hopelessly confuse GDB.  So, we
must start using GDB in assembly language, if something goes wrong here.

If 'setcontext' did not work for you, then you will need two advanced
techniques (GDB with assembly; and restoring the symbol table of 'hello'
from the restored memory segments so that GDB can recognize the source
code of 'hello')

1.  DEBUGGING AT ASSEMBLY LEVEL with GDB:
    There are three additional GDB commands to learn in order to
    debug in GDB:
  a. Equivalent of 'list' command for assembly level:
     (gdb) x/10i $pc
     [This means: _eXamine_ ('x') the next _10 instructions_ ('x/10i'),
      starting with the value of the program counter ('$pc').  Try this on
      any GDB session, and it will quickly become intuitive.]
     Alternativelyi, to repeat after each command:
     (gdb) display/10i $pc
  b. The equivalents of 'step' and 'next commands for assembly level are:
     They are 'si' ("step (assembly) instruction") and
     'ni' ("next assembly instruction").
     (And 'si 8) means to step for 8 assembly instructions.)
     As you step through 'setcontext', eventually you will jump or 'return'
     into the middle of the assembly code for 'getcontext'.  This will be
     obvious because the address of the next assembly instruction will
     change a lot (from the memory segment of 'setcontext' in 'restart'
     to the memory segment of 'getcontext' in libckpt.so).
  c. (gdb) p/x $pc
     [This means print ('p') in heXadecimal ('p/x') the program
      counter ('$pc').  Similarly, you can print the stack pointer ('$sp').]
  d. (gdb) info registers
     [The name says it all.  Display current values of all registers.]
  e. Once you have arrived inside getcontext/libckpt.so, one more sanity
     text is useful.  Try:
     (gdb) x/100x $sp      # Examine the next 100 words (in hex) on the stack.
     If you see mostly zeroes, then your stack has a problem.

2.  LOADING A NEW SYMBOL TABLE into GDB:
        After 'restart' has mapped in the memory of the user application
    (e.g., hello.c), we now want to see the symbols of 'hello.c' instead
    of the symbols of 'restart'.  So, we need to do
        Recall that a symbol table is a table in which each row specifies
    a symbol (variable name, function name, etc.), followed by
    the address where it is stored in RAM, and optionally followed
    by the type of the symbol (or at least the size of the memory
    buffer that the address points to, such as 4 bytes for an 'int').

    For background:
    (gdb) apropos symbol
      add-symbol-file -- Load symbols from FILE
      ...
    The 'add-symbol-file' command requires a filename and an address:
    (gdb) help add-symbol-file
      ...
      Usage: add-symbol-file FILE ADDR [...]
      ADDR is the starting address of the file's text segment.

    So, the hard part is to find the address of the file's text segment.
    For that, do:
    (gdb) shell readelf -S hello | grep text
    [ This shows both the absolute address, and the offset into the
      text memory segment in case your 'hello' was loaded to a different
      absolute address than the default. ]
    So, the command to load the symbol table of 'hello' is:
    (gdb) add-symbol-file ./hello 0x4006e0
    [ assuming that 'readelf' produced '0x4006e0' ]
    You'll probably also want to load the symbol table for the text segment
    for your 'libckpt.so'.  For this, do:
    (gdb) shell readelf -S libckpt.so | grep text
    (gdb) add-symbol-file ./libckpt.so 0x7ffff7bd9960
    [ In this case, 'readelf' returned only the offset, since it could
      be loaded into any absolute address.  We are assuming that the
      offset of the libckpt.so test segment was 0x960.  We still needed
      to know the absolute address where the text segment of libckpt.so
      was actually loaded.  But you can get that using your
      own command 'readckpt myckpt'.  We assume here that 'readckpt myckpt'
      showed that the memory segment of libckpt.so in 'hello' was loaded
      with a start address for the memory segment of:  0x7ffff7bd9000.
      So, 0x7ffff7bd9000 + 0x960 yields the 0x7ffff7bd9960 that we used. ]
    Hopefully, you won't need to refer to the global variables of your
    data segment after executing 'setcontext'.  But if you do need that,
    then look at '(gdb) help add-symbol-file' and use '-s data' in that
    GDB command, and follow the original recipe with 'data' instead of 'text'..

==================
APPENDIX: G. ADVANCED TOPIC: HOW TO PORT THIS FOR RECENT Linux DISTROs (kernels)

The scheme described so far works well in CentOS 7 (Linux kernel 3.10).
If you want to extend this project to work on a recent Linux distro,
then you will have to add a bit more code in order to make 'restart'
work correctly.

The issue is how an application makes a system call involving the kernel.

A. At assembly level, an application calls the kernel using the
   assembly instruction 'syscall'.  The '$rax' register holds the
   system call numbers (as seen for MPIPS/MARS and for xv6).  For
   further reading, see:
     https://github.com/torvalds/linux/blob/master/arch/x86/entry/entry_64.S

B. Certain system calls are special, such as gettimeofday().  The kernel
   creates a virtual memory segment (labelled "[vdso]" in /proc/*/maps).
   The code to execute the system call is kept in user-space (in 'vdso').
   For example, in the case of gettimeofday(), the kernel uses virtual
   memory and other tricks to place the value for the hardware timer
   in the middle of the vdso segment, so that a kernel call is not required.
     For details about 'vdso', read "man vdso".

C. In recent kernels, the address of the vdso segment is randomized
   when a program is loaded (via the exec() family).  Here is a
   URL talking about fixing a kernel bug in the address randomizationn
   of vdso (as of May, 2015):  https://lore.kernel.org/patchwork/patch/943579/

The solution to extend your checkpointing project for recent kernels
is, just before calling setcontext(), to call 'mremap' (see 'man mremap')
to move the vdso/vvar/vsyscall segments from their address in the
'restart' program to their original address in the 'hello' program.
(This assumes that the vdso/vvar/vvsyscall addresses were also saved
 in 'myckpt'.  For example, you might include the address in headers
 for vdso/vvar/vsyscall, but add an extra header field, 'no_checkpoint'.)

Additionally, you may need to use the 'gcc' flag '-fno-stack-protector',
in order to stop glibc from complaining about "*** stack smashing detected ***".
This is especially important for the C files in 'libckpt.so' and 'restart'.
"Stack protection" refers to a default scheme in modern glibc, for
detecting buffer overruns by malware.  But that is another story, entirely.

And so, in conclusion, recent Linux distros have added more "features",
which force one to work harder to support transparent checkpointing.
Yet, with some amount of extra work, transparent checkpointing can
still be accomplished.  Two current packages to support this are:
DMTCP and CRIU.  The differences in those two approaches would also
be another story, entirely.
