#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_COMMANDS 100
#define MAX_LINE 100
#define PIPE_READ 0
#define PIPE_WRITE 1

void parse_command(char buffer[], char *command[]);
void check_special_character(char *command, int index);
void exec_background(char *command[]);
void exec_redirect(char *command[]);
void exec_pipe(char *command[]);
void get_commands(char* new_command[], char* command[], int start, int end);

pid_t childpid = 0;
int is_background = 0;
int is_redirect = 0;
int is_pipe = 0;
int special_character_index = 0;
int total_command_words = 0;

void signal_handler(int sig) {
  if(childpid != 0) {
    // Kill the child process
    printf("\n");
    kill(childpid, SIGKILL);
    childpid = 0;
  } else {
    printf("\n%% ");
    fflush(stdout);
  }
}

int main(int argc, char* argv[]) {
  //Setting the SIGINT signal handler
  signal(SIGINT, &signal_handler);

  while (1) {
    childpid = 0;
    is_background = 0;
    is_redirect = 0;
    is_pipe = 0;
    special_character_index = 0;
    total_command_words = 0;
    char *command[MAX_COMMANDS];  // For commands with up to 100 words
    printf("%% ");
    fflush(stdout);

    // Read a string from command line:
    /* fgets creates null-terminated string. stdin is pre-defined C constant
     * for standard intput.  feof(stdin) tests for file:end-of-file.
     */
    char buffer[MAX_LINE];  // For commands with up to 100 characters
    if (fgets(buffer, MAX_LINE, stdin) == NULL && feof(stdin)) {
      printf("Couldn't read from standard input. End of file? Exiting ...\n");
      exit(1);  /* any non-zero value for exit means failure. */
    }

    // To Fill in 'command' with pointers to words (char *), followed
    // by a NULL pointer.  'execvp' requires that the last pointer be NULL.
    parse_command(buffer /* input */, command /* output */);

    if(total_command_words == 0) {
      //If input is empty
      continue;
    }
    if(strcmp(command[0], "quit") == 0) {
      //Special command to exit mybash. % quit
      return 0;
    }

    childpid = fork();
    //printf("%d\n", childpid);
    if (childpid == -1) {
      perror("fork");
    } else if (childpid == 0) {
      //In child Process

      if(is_background) {
        exec_background(command);
      } else if(is_redirect) {
        exec_redirect(command);
      } else if(is_pipe) {
        exec_pipe(command);
      } else {
        if (-1 == execvp(command[0], command)) {
          perror("execvp");
        }
      }
      return 0;
    } else {  // else parent
      int status;
      if (!is_background) {
        if(waitpid(childpid, &status, 0) == -1) {
          perror("waitpid");
        }
      }
    }
  }
}

void parse_command(char buffer[], char *command[]) {
  int index = 0;
  int i = 0;

  int length = strlen(buffer);

  //Removing '\n' from buffer
  buffer[length-1] = '\0';

  //Using strtok, see man 3 strtok
  command[index] = strtok(buffer, " ");

  while( command[index] != NULL ) {
      //Checking for special characters: &, | or >
      check_special_character(command[index], index);
      index++;
      //Using NULL to continuing tokenizing previous string
      command[index] = strtok(NULL, " ");
  }
  //Background only if last command
  if(is_background) {
    if(special_character_index != index-1) {
      is_background = 0;
    }
  }
  total_command_words = index;
}

void check_special_character(char *command, int index) {
  //Checking for special characters
  if(strcmp(command, "&") == 0) {
    is_background = 1;
    special_character_index = index;
  } else if(strcmp(command, ">") == 0) {
    is_redirect = 1;
    special_character_index = index;
  } else if(strcmp(command, "|") == 0) {
    is_pipe = 1;
    special_character_index = index;
  }
}

void exec_background(char* command[]) {
  //Executing background

  //Extracting required commands for background
  char* new_command[100];
  get_commands(new_command, command, 0, total_command_words-2);
  if (execvp(new_command[0], new_command) == -1) {
    perror("execvp");
  }
}

void exec_redirect(char* command[]) {
  //Extracting required commands for redirection
  char* new_command[100];
  get_commands(new_command, command, 0, special_character_index-1);

  //Extracting filename for redirection
  char* file_command[100];
  get_commands(file_command, command, special_character_index+1, total_command_words-1);
  if(file_command[1] != NULL) {
    fprintf(stderr, "Incorrect file commands for redirection\n");
  }

  //Opening the file
  int fd = open(file_command[0], O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if(fd == -1) {
    perror("open");
  }

  //Closing stdout
  if(-1 == close(STDOUT_FILENO)) {
    perror("close");
  }

  //dup fd to set as stdout
  int newfd = dup(fd);
  close(fd);
  if (-1 == execvp(new_command[0], new_command)) {
    perror("execvp");
  }
}

void exec_pipe(char* command[]) {
  //Simmilar to pipe-example.c
  int pipe_fd[2];
  int fd;
  pid_t child1, child2;
  char* argvChild[2];

  if (-1 == pipe(pipe_fd)) {
    perror("pipe");
  }
  child1 = fork();
  if (child1 > 0) {
    child2 = fork();
  }
  if (child1 == 0) {
    if(-1 == close(STDOUT_FILENO)) {
      perror("close");
    }
    fd = dup(pipe_fd[PIPE_WRITE]);
    if (-1 == fd) {
      perror("dup");
    }

    if (fd != STDOUT_FILENO) {
      fprintf(stderr, "Pipe output not at STDOUT.\n");
    }

    close(pipe_fd[PIPE_READ]);
    close(pipe_fd[PIPE_WRITE]);


    char* new_command[100];
    get_commands(new_command, command, 0, special_character_index-1);
    if (-1 == execvp(new_command[0], new_command)) {
      perror("execvp");
    }
  } else if (child2 == 0) {
    if (-1 == close(STDIN_FILENO)) {
      perror("close");
    }
    fd = dup(pipe_fd[PIPE_READ]);
    if (-1 == fd) {
      perror("dup");
    }
    if(fd != STDIN_FILENO) {
      fprintf(stderr, "Pipe input not at STDIN.\n");
    }
    close(pipe_fd[PIPE_READ]);
    close(pipe_fd[PIPE_WRITE]);

    char* new_command[100];
    get_commands(new_command, command, special_character_index+1, total_command_words-1);
    if(-1 == execvp(new_command[0], new_command)) {
      perror("execvp");
    }
  } else {
    int status;

    close(pipe_fd[PIPE_READ]);
    close(pipe_fd[PIPE_WRITE]);

    if (-1 == waitpid(child1, &status, 0)) {
      perror("waitpid");
    }

    if (WIFEXITED(status) == 0) {
      printf("child1 returned w/ error code %d\n", WEXITSTATUS(status));
    }

    if (-1 == waitpid(child2, &status, 0)) {
      perror("waitpid");
      }

    if (WIFEXITED(status) == 0) {
      printf("child1 returned w/ error code %d\n", WEXITSTATUS(status));
    }

    if (-1 == waitpid(child2, &status, 0)) {
      perror("waitpid");
    }


    if (WIFEXITED(status) == 0) {
      printf("child2 returned w/ error code %d\n", WEXITSTATUS(status));
    }
  }

}

void get_commands(char* new_command[], char* command[], int start, int end) {  //Get the commands from start index to end index
  int i = 0;
  int j = start;
  while(j<=end) {
    new_command[i] = command[j];
    i++;
    j++;
  }
  new_command[i] = NULL;
}
