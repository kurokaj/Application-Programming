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


int main()
{
  printf("This is the testing program\n");
  printf("It creates 20 child processes to access the server 'at the same time'.\n");


  pid_t pid;
  // fork 5 childs to exec transmitter.c program with each child
  for(int i = 1; i <= 20; i++)
  {
      // create new child
      pid = fork();
      // if child was succesfully created, exec program helloworld
      if(pid == 0)
      {
          char *args[]={"./transmitter","ftoread.txt",NULL};
          execvp(args[0],args);
          // if child gets here the execvp has failed
          printf("The execution of child %d failed.\n", i);
          return 0;
      }
      else if(pid > 0){ // The parent node
          sleep(0.1);
          continue;
      }
      else{ // fork failed
        perror("The fork failed.\n");
        return -1;

      }
  }

  // Wait that all children have returned succesfully
  for(int i = 1; i <= 20; i++){
      if(pid != 0){
          if(waitpid(-1, NULL, 0) <= 0){
              perror("The waitpid function failed");
          }
      }
  }

  printf("All children have returned, exiting the tester.\n");

  return 0;

}
