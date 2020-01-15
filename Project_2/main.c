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
#include <pthread.h>
#include <assert.h>
#include <time.h>

#include "log.h"
#define BUFSIZE 128
#define WAIT_TIME_SECONDS       4
#define FIFO_PERMS (S_IRWXU | S_IWGRP| S_IWOTH)

// log file descriptor
static int log_fd;
static FILE *output_fd;
//pipe file descriptor for sending permissions
static int fd;
//pipe file descriptor for receiving the initial contact from A
static int id_fd;
//pipe file descriptor for receiving the message from A
static int text_fd;
int buflen;
int buflen_id;
int clients = 0; // The number of clients in the process
char delim[] = ","; // Separator for the multiple clients that are connecting simultaneously
char *ptr; // For the different clients that may try to connect at the same time
int success = 0; // Number of customers served successfully
int flag; // Whole message received or not

// Buffers for messages
char buf[BUFSIZE];
char buf_id[BUFSIZE];

pthread_t thread_id;
pthread_mutex_t lock;
// The condition for the mutex lock
// Could not get this working properly to serve processes in parallel
pthread_cond_t cond;
// timer values for the parallel serving
time_t start_t, end_t;

void sig_int(int s)
{
    switch(s)
    {
        //check only SIGINT
        case SIGINT:
        {
            printf("Process will exit as user pressed (ctrl + c)\n");

            log_write(log_fd, "The user pressed ctrl+C, terminating the program.\n");

            // Close the files
            close(log_fd);
            fclose(output_fd);

            // Destroy the mutex lock
            pthread_mutex_destroy(&lock);
            // Close the pipes
            close(fd);
            close(id_fd);
            close(text_fd);

            printf("The number of served clients: %d.\n", success);
            printf("All programs were terminated. See ya!\n");
            exit(0);
        }
        // Else execute the program
        default:
            break;
    }
}

// The thread function
void *thread_func(void *arg){

  printf("{%d} Thread blocked..\n", (unsigned int) pthread_self());

  /////////////////////////////////// When lock is opened, the process is served
  pthread_mutex_lock(&lock);

  // The timed condition for mutex lock
  //pthread_cond_wait(&cond, &lock);

  sleep(0.1); // for the A Process to catch up

  // The pid of the process that is served
  char *pid = (char *)arg;
  printf("{%d} Lock opened. Thread continues with argument [%s]..\n", (unsigned int) pthread_self(), pid);


  // Open pipe to tell client it is served
  if ((fd = open("./fifopipe", O_WRONLY)) == -1) {
    perror("Failed to open FIFO");
    exit(0);
  }
  // Send permission to A Process
  write(fd, pid ,strlen(pid));

  printf("{%d} Thread gives process [%s] the rigth to write. \n",(unsigned int) pthread_self(),pid);
  close(fd);

  // Open the pipe to receive the file text
  if ((text_fd = open("./textpipe", O_RDONLY|O_NONBLOCK)) == -1) {
    perror("Failed to open FIFO");
    exit(0);
  }

  // Loop the reading one char at a time until # is received as a sign that no more text is coming
  while(flag!='#'){
    buflen = read(text_fd, &flag, sizeof(int));
    if(buflen>0){
      fprintf(output_fd, "%c",flag);
    }
  }
  flag = '\n';
  fprintf(output_fd,"%c",flag);

  char dip[40];
  sprintf(dip,"Message from [%s] delivered...\n", pid);
  log_write(log_fd, dip);

    success++;
    clients--;
   close(text_fd);
   printf("{%d} Closing thread!\n", (unsigned int) pthread_self());

   ////////////////////////////////// Unlock the mutex, the served process is done writing
   pthread_mutex_unlock(&lock);
   pthread_exit(arg);
}

int main()
{
  // To handle the termination signal
  struct sigaction sact;
  sigemptyset(&sact.sa_mask);
  sact.sa_handler = &sig_int;
  sact.sa_flags = 0;
  sigaction(SIGINT, &sact, NULL);
  sigaction(SIGTERM, &sact, NULL);

  // Initialize the mutex
  if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

  // remove the old log, if exists, and open new one
  remove("receiver.log");
  // remove old outputfile, if exists, and open new one
  remove("outputfile.txt");

  // open .log file with tags below
  log_fd = open("receiver.log", O_RDWR | O_APPEND | O_CREAT | O_NONBLOCK, S_IRWXU);
  if (log_fd == 0)
  {
      perror("Error: \n");
      return -1;
  }
  // open the outputfile for the received messages
  output_fd = fopen("outputfile.txt","w");
  if (output_fd == NULL)
  {
      perror("Error: \n");
      return -1;
  }

  printf("This is the data transfer app.\n");
  log_write(log_fd, "The main program was started.\n");

  // Create pipes for the program B
   log_write(log_fd,"Creating pipe-file for communication and initial contact\n");
   if ((mkfifo("./fifopipe", FIFO_PERMS) == -1) && (errno != EEXIST))
   {
       perror("Failed to create a FIFO");
       return 1;
   }
   if ((mkfifo("./idpipe", FIFO_PERMS) == -1) && (errno != EEXIST))
   {
       perror("Failed to create a FIFO");
       return 1;
   }
   if ((mkfifo("./textpipe", FIFO_PERMS) == -1) && (errno != EEXIST))
   {
       perror("Failed to create a FIFO");
       return 1;
   }

   log_write(log_fd, "Creation of all pipe-files was succesfull..\n");

   printf("Opened ID pipe to get clients ID!!\n");

   // Opened ID- pipe as Non_blocking
   if ((id_fd = open("./idpipe", O_RDONLY|O_NONBLOCK)) == -1) {
     perror("Failed to open FIFO\n");
     return 1;
   }


   //time(&start_t);
   while(1){
     sleep(1);
     printf("Waiting for new client...Amount of clients in the system: [%d].\n", clients);

     buflen_id = read(id_fd, buf_id, BUFSIZE);

     if(buflen_id > 0){
          printf("Client joined the server.\n");
          ptr = strtok(buf_id, delim);
          while(ptr != NULL)
          {
              clients++;
              printf("Opening thread for program [%s]\n",ptr);

              assert((pthread_create(&thread_id,NULL,thread_func,(void *) ptr)) == 0);
              ptr = strtok(NULL, delim);
              sleep(0.5); // The opening of threads fuks up for some reason without
          }

       }
      else{
          continue;
      }
      // The timed parallel serving, could not get this to work
      /*
      time(&end_t);
      if(clients != 0 && difftime(end_t, start_t)>=WAIT_TIME_SECONDS){
          printf("Permission to write for the next process!\n");
          pthread_mutex_lock(&lock);
          // reopen the mutex on one thread
          pthread_cond_signal(&cond);

          pthread_mutex_unlock(&lock);
          time(&start_t);
        }
      */
    }

      // Destroy the mutex lock
      pthread_mutex_destroy(&lock);
      close(id_fd);
      close(log_fd);
      close(fd);
      return 0;

}
