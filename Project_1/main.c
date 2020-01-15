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

#include "log.h"

static int log_fd;

void sig_int(int s)
{
    switch(s)
    {
        //check only SIGINT
        case SIGINT:
        {
            printf("Process will exit as user pressed (ctrl + c)\n");

            log_write(log_fd, "The user pressed ctrl+C, terminating the program.");

            // send SIGTERM to every child processes with same ID groub and kill them
            kill(0, SIGTERM);

           pid_t pid_clo;
           int status = 0;
           // loop until all children have safely exited
           while ((pid_clo = wait(&status)) > 0){
             sleep(0.1);
           }

            log_write(log_fd, "All children have returned, exiting the main program!");
            printf("All programs were terminated. See ya!\n");

            exit(0);
        }
        // Execute the program as well
        default:
            break;
    }
}

int main()
{

  // To handle the signals
  struct sigaction sact;
  sigemptyset(&sact.sa_mask);
  sact.sa_handler = &sig_int;
  sact.sa_flags = 0;
  sigaction(SIGINT, &sact, NULL);
  sigaction(SIGTERM, &sact, NULL);


  // remove the old log, if exists and open new one
  remove("helloworld.log");
  // open .log file with talgs below
  log_fd = open("helloworld.log", O_RDWR | O_APPEND | O_CREAT | O_NONBLOCK, S_IRWXU);
  if (log_fd == 0)
  {
      perror("Error: \n");
      return -1;
  }

  printf("Welcome to multiworld visiting program.\n");

  log_write(log_fd, "The main program was started.");

  // read travel plan (config. file) from a text file
  FILE *travelplan = fopen("travelplan.txt", "r");
  if (travelplan == NULL)
  {
      perror("Error: \n");
      return -1;
  }
  char buf[100];
  fgets(buf, 100, travelplan);

  int number;
  if(sscanf(buf, "The number of random worlds to explore: %d", &number) == 1){
    printf("The travelplan was opened succesfully and we are going to visit %d worlds\n", number);
  }
  else{
    printf("The travelplan was not open succesfully, exiting program.");
    return -1;
  }

  pid_t pid;
  // fork X childs (determined in confif. file), exec new_world program with each child
  for(int i = 1; i <= number; i++)
  {
      // create new child
      pid = fork();
      // if child was succesfully created, exec program helloworld
      if(pid == 0)
      {
          char *args[]={"./new_world",NULL};
          execvp(args[0],args);
          // if child gets here the execvp has failed
          printf("The execution of child %d failed.\n", i);
          return 0;
      }
      else if(pid > 0){ // The parent node
          // wait a bit before creating another child process
          sleep(1);
      }
      else{ // fork failed
        perror("The fork failed.\n");
        return -1;

      }
  }

  // Wait that all children have returned succesfully
  for(int i = 1; i <= number; i++){
      if(pid != 0){
          if(waitpid(-1, NULL, 0) <= 0){
              perror("The waitpid function failed");
          }
      }
  }

  printf("All children have returned, exiting the main program.\n");
  log_write(log_fd, "All children have returned, exiting the main program!\n");
  close(log_fd);

  return 0;

}
