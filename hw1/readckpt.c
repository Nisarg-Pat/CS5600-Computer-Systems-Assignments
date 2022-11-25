#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "helper.c"
#include <ucontext.h>

int main(int argc, char* argv[]) {
  printf("****IN READCKPT****\n");
  if(argc !=2) {
    printf("Illegal arguments. Format: ./readckpt myckpt\n");
    exit(1);
  }
  int fd = open(argv[1], O_RDONLY);
  struct proc_maps_header proc_maps[1000];
  int i = 0;
  ucontext_t context;
  read_helper(fd, &context, sizeof(ucontext_t));
  printf("Context flags: %lu\n", context.uc_flags);
  while(read_helper(fd, &proc_maps[i], sizeof(struct proc_maps_header)) > 0) {
    //printf("%d\n", i);
    //printf("%d\n", proc_maps[i].end - proc_maps[i].start);
    void *addr = mmap(NULL, proc_maps[i].end - proc_maps[i].start,
                    PROT_READ|PROT_WRITE|PROT_EXEC,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    int k = read_helper(fd, addr, proc_maps[i].end-proc_maps[i].start);
    //printf("Actual Read: %d\n", k);
    i++;
  }
  printf("    *** Memory segments ***\n");
  for (i = 0; proc_maps[i].start != NULL; i++) {
  printf("%s (%d%d%d)\n"
           " Address-range: %p - %p\n",
                     proc_maps[i].name,
                      proc_maps[i].read, proc_maps[i].write, proc_maps[i].execute,
                         proc_maps[i].start, proc_maps[i].end);
  }
  close(fd);
}
//Based on this code, memory segments agree when compared to /proc/PID/maps
//The file size is comparable(exact) to the sum of memory segments + sum of header size + size of ucontext_t
