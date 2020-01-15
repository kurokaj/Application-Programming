#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>

#include "hellolib.h"
#include "log.h"

static int log_fd;

void sig_term(int s)
{
    switch(s)
    {
      // only SIGTERM, exit program if SIGTERM
      case SIGTERM:
          log_write(log_fd, "The main process send SIGTERM, closing child...");
          printf("Child %d is killed, as it received SIGTERM.\n", getpid());
          exit(0);
      default:
          sleep(1); // is there better way to ignore the SIGINT?
          return;
      }
}

int main()
{
  // open the log-file
  log_fd = open("helloworld.log", O_RDWR | O_APPEND | O_CREAT | O_NONBLOCK, S_IRWXU);

  // for the sig_term
  struct sigaction sact;
  sigemptyset(&sact.sa_mask);
  sact.sa_handler = &sig_term;
  sact.sa_flags = 0;
  sigaction(SIGTERM, &sact, NULL);
  sigaction(SIGINT, &sact, NULL);

  int pid_n = getpid();

  // for building the .log message
  char log[1000] = { 0 };
  sprintf(log, "A new world visited! The world PID is %d", getpid());
  log_write(log_fd, log);

  // "Visiting new world" -message
  if(hello(pid_n)!=1){
    perror("Error executing the hello-function, exiting program.\n");
    return -1;
  }

  // wait time to see that terminating child processes works
  sleep(2);

  // "Leaving the world" -message
  if(byebye(pid_n)!=1){
    perror("Error executing the byebye-function, exiting program.\n");
    return -1;
  }

  return 0;
}
