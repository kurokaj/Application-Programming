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
#include <sys/file.h>

#define BUFSIZE 128

// The file descriptors
int fd;
int fd_id;
int fd_text;
FILE *file;
// Character to be sent
int ch;
// Some buffer lenghts
int buflen;
int buflen_id;
// Buffers for sending and receiving
char buf_id[BUFSIZE];
char buf[BUFSIZE];

void sig_int(int s)
{
    switch(s)
    {
        //check only SIGINT
        case SIGINT:
        {
            printf("Process will exit as user pressed (ctrl + c)\n");

            printf("All programs were terminated. See ya!\n");

            // Close files
            fclose(file);
            close(fd);
            close(fd_id);
            close(fd_text);

            exit(0);
        }
        // Execute the program normally
        default:
            break;
    }
}

int main(int argc, char **argv)
{
  // For the termination signal
  struct sigaction sact;
  sigemptyset(&sact.sa_mask);
  sact.sa_handler = &sig_int;
  sact.sa_flags = 0;
  sigaction(SIGTERM, &sact, NULL);
  sigaction(SIGINT, &sact, NULL);

  // Open text file that will be read
  file = fopen(argv[1], "r");
  if (file == NULL)
  {
      perror("Error: \n");
      return -1;
  }

  // Open the ID pipe for the initial contact to the server
  if ((fd_id = open("./idpipe", O_WRONLY)) == -1)
      {
        perror("Failed to open FIFO_ ID");
        return 1;
    }

  // File- lock to the id-pipe so that only one A Processat a time can contact server
  // Blocks until is free again
  if (flock(fd_id, LOCK_EX) != 0) {
      printf("[%d] The process could not lock the file!!!\n",getpid());
    }

  // Send ID (Comma separated if multiple process tries simultaneously)
  char pid_c[10] = { 0 };
  // Real PID without comma
  char pid[10] = { 0 };
  sprintf(pid_c, "%d,", getpid());
  sprintf(pid, "%d", getpid());
  write(fd_id, pid_c, strlen(pid_c));

  // Unlock the id_pipe for others to use
  if (flock(fd_id, LOCK_UN) != 0) {
      printf("[%d] Failed to release the lock!!!\n", getpid());
    }

  close(fd_id);

  // Open pipe to receive permission to write
  if ((fd = open("./fifopipe", O_RDONLY|O_NONBLOCK)) == -1)
      {
        perror("Failed to open FIFO_text");
        return 1;
      }

  printf("[%d] Waiting permission to send text...\n", getpid());

  // Wait until own pid is called
  buflen_id = read(fd, buf_id, strlen(pid));
  while(*buf_id!=*pid){
    buflen_id = read(fd, buf_id, strlen(pid));
  }

  printf("[%d] Permission to write received, starting com. pipe...\n", getpid());

  // Open a write communication endpoint to the text pipe
  if ((fd_text = open("./textpipe", O_WRONLY)) == -1)
      {
        perror("Failed to open FIFO");
        return 1;
      }

  // Read the message from file one character at a time
  while ((ch = fgetc(file)) != EOF){
    // Checks that the permission is still effective
    // If not waits for the own pid
    buflen_id = read(fd, buf_id, strlen(pid));
    if(buflen>0){
        while(*buf_id!=*pid){
            buflen_id = read(fd, buf_id, strlen(pid));
        }
        printf("[%d] New permission to write catched, continue writing...\n", getpid());
      }
    write(fd_text, &ch, sizeof(int));
  }

    // Close file descriptors
   fclose(file);
   close(fd);
   close(fd_text);

   printf("[%d] A Program done sending through pipe.. \n",getpid());

   printf("[%d] The A program has terminated..\n",getpid());

   return 0;
}
