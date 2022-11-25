#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


#define NAME_LEN 80
struct ckpt_header {
  void *start;
  void *end;
  int read, write, execute;
  int is_context;
  char name[NAME_LEN];
};

void print_ckpt_header(struct ckpt_header header) {
  if(header.is_context == 1) {
    printf("Context Header: %s, is_context: %d\n", header.name, header.is_context);
  } else {
    printf("%s (%d%d%d)\n"
           " Address-range: %p - %p\n",
                     header.name,
                      header.read, header.write, header.execute,
                         header.start, header.end);
  }
}

void write_helper(int fd, const void* buf, size_t count){
  //printf("Required: %d\n", count);
  //printf("%p\n", buf);
  int rc = write(fd, buf, count);
  if (rc == -1) {
    perror("write");
    exit(0);
  }
  //printf("Original: %d\n", rc);
  while (rc < count) {
    rc += write(fd, (char *)buf + rc, count - rc);
    //printf("%d\n", rc);
  }
  assert(rc == count);
}

int read_helper(int fd, void* buf, size_t count) {
  int rc = read(fd, buf, count);
  if (rc == 0) {
    return 0;
  }
  if (rc == -1) {
    perror("read");
    exit(0);
  }
  while (rc < count) {
    rc += read(fd,(char *)buf + rc, count - rc);
    //printf("%d\n", rc);
  }
  assert(rc == count);
  return rc;
}
