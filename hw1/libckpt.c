#define _GNU_SOURCE /*Required for 'constructor' attribute (GNU extention) */
#include <stdio.h>
#include <fcntl.h>
#include <ucontext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <stdlib.h>

#include "helper.c"

void checkpoint();
#define SPECIAL_SEGMENTS -999

ucontext_t context;

void signal_handler(int sig) {
  checkpoint();
}

void __attribute((constructor))
my_constructor() {
  signal(SIGUSR2, &signal_handler);
  printf("********** HELLO FROM CONSTRUCTOR ***********\n");
}

void save_memory(int fd, struct proc_maps_header proc_maps[]);

void checkpoint() {
  struct proc_maps_header proc_maps[1000];
  assert( proc_self_maps(proc_maps) == 0 );
  //assert( proc_self_maps(proc_maps) == 0 );
  //printf("    *** Memory segments ***\n");
  //int i = 0;
  //for (i = 0; proc_maps[i].start != NULL; i++) {
  //  printf("%s (%d%d%d)\n"
  //         "  Address-range: %p - %p\n",
  //         proc_maps[i].name,
  //         proc_maps[i].read, proc_maps[i].write, proc_maps[i].execute,
  //         proc_maps[i].start, proc_maps[i].end);
  //}
  int fd = open("myckpt", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fd == -1) {perror("open");}
  save_memory(fd, proc_maps);
  close(fd);
}

int match_one_line(int proc_maps_fd,
                   struct proc_maps_header *proc_maps_line, char *filename) {
  unsigned long int start, end;
  char rwxp[4];
  int read = 0;
  int write = 0;
  int execute = 0;
  char tmp[10];
  int tmp_stdin = dup(0);
  dup2(proc_maps_fd, 0);
  int rc = scanf("%lx-%lx %4c %*s %*s %*[0-9 ]%[^\n]\n",
                 &start, &end, rwxp, filename);
  //printf("%s\n%p\n%p\n%c%c%c%c\n", filename, start, end, rwxp[0], rwxp[1], rwxp[2], rwxp[3]);
  assert(fseek(stdin, 0, SEEK_CUR) == 0);
  dup2(tmp_stdin, 0);
  close(tmp_stdin);
  if (rc == EOF || rc == 0) {
    proc_maps_line -> start = NULL;
    proc_maps_line -> end = NULL;
    return EOF;
  }
  if(rwxp[0] == 'r') {
    read = 1;
  }
  if(rwxp[1] == 'w') {
    write = 1;
  }
  if(rwxp[2] == 'x') {
    execute = 1;
  }
    if(read == 0 && write == 0 && execute == 0) {
    return SPECIAL_SEGMENTS;
  }
  if(strcmp(filename, "[vsyscall]")==0 || strcmp(filename, "[vvar]")==0) {
    return SPECIAL_SEGMENTS;
  }

  if (rc == 3) {
    strncpy(proc_maps_line -> name,
            "ANONYMOUS_SEGMENT", strlen("ANONYMOUS_SEGMENT")+1);
  } else {
    assert( rc == 4 );
    strncpy(proc_maps_line -> name, filename, NAME_LEN-1);
    proc_maps_line->name[NAME_LEN-1] = '\0';
  }
  proc_maps_line -> start = (void *)start;
  proc_maps_line -> end = (void *)end;
  proc_maps_line -> read = read;
  proc_maps_line -> write = write;
  proc_maps_line -> execute = execute;
  return 0;
}

int proc_self_maps(struct proc_maps_header proc_maps[]) {
  int proc_maps_fd = open("/proc/self/maps", O_RDONLY);
  //printf("IN PROC_SELF_MAPS\n");
  char filename [100];
  int i = 0;
  int rc = -2;
  for (i=0; rc != EOF;) {
    rc = match_one_line(proc_maps_fd, &proc_maps[i], filename);
    if(rc != SPECIAL_SEGMENTS) {
      i++;
    }
  }
  close(proc_maps_fd);
  return 0;
}

void save_memory(int fd, struct proc_maps_header proc_maps[]) {
  getcontext(&context);
  write_helper(fd, &context, sizeof(ucontext_t));
  int i = 0;
  for (i = 0; proc_maps[i].start != NULL; i++) {
    //write_helper();
    write_helper(fd, &proc_maps[i], sizeof(struct proc_maps_header));
    write_helper(fd, proc_maps[i].start, proc_maps[i].end - proc_maps[i].start);
    //printf("%d\n", proc_maps[i].end - proc_maps[i].start);
  }
}
