#include<stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "helper.c"
#include <ucontext.h>

void do_work();
void recurse(int level);
void restore_memory();

ucontext_t context;

int main(int argc, char* argv[]) {
  recurse(1000);
  return 0;
}

void recurse(int level) {
  if(level>0) {
    recurse(level-1);
  } else {
    do_work();
  }
}

void do_work() {
  //printf("Hello\n");
  restore_memory();
  setcontext(&context);
  //printf("Hello2\n");
}

void restore_memory() {
  int fd = open("myckpt", O_RDONLY);
  struct ckpt_header proc_maps[1000];
  struct ckpt_header context_header;
  int i = 0;
  //ucontext_t context;
  read_helper(fd, &context_header, sizeof(struct ckpt_header));
  //printf("SIZE!!::::%d::::::::!!\n", sizeof(ucontext_t));
  read_helper(fd, &context, sizeof(ucontext_t));
  //print_ckpt_header(context_header);
  //printf("Context flags: %lu\n", context.uc_flags);
  while(read_helper(fd, &proc_maps[i], sizeof(struct ckpt_header)) > 0) {
    //printf("%d\n", i);
    //printf("%d\n", proc_maps[i].end - proc_maps[i].start);
    // MAP_PRIVATE corresponds to the 'p' in things like 'r-xp' in /proc/*/maps
    // MAP_ANONYMOUS means that this memory has no backing file.  So, in
    //   /proc/*/maps, there will be no filename associated with this segment.
    // We will need execute permission if we want to load code at this address.

    //void *addr = mmap(NULL, FNC_LEN,
    //                PROT_READ|PROT_WRITE|PROT_EXEC,
    //                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    //print_ckpt_header(proc_maps[i]);
    int flag = 0;
    if(proc_maps[i].read == 1) {
      flag = flag | PROT_READ;
    }
    if(proc_maps[i].write == 1) {
      flag = flag | PROT_WRITE;
    }
    if(proc_maps[i].execute == 1) {
      flag = flag | PROT_EXEC;
    }
    void *addr = mmap(proc_maps[i].start, proc_maps[i].end - proc_maps[i].start,
                    PROT_READ|PROT_WRITE|PROT_EXEC, //flag,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if(addr == MAP_FAILED) { perror("mmap"); }
    int k = read_helper(fd, addr, proc_maps[i].end-proc_maps[i].start);
    //mprotect(proc_maps[i].start, proc_maps[i].end - proc_maps[i].start, flag);
    //printf("Actual Read: %d\n", k);
    i++;
  }
  //printf("    *** Memory segments ***\n");
  //for (i = 0; proc_maps[i].start != NULL; i++) {
  //  print_ckpt_header(proc_maps[i]);
  //}
  close(fd);
}
