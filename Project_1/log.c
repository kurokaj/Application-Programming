#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "log.h"

void log_write(int fd, char *text)
{
  // to print the message and time
  char log[1000] = { 0 };
  time_t t = time(NULL);
  struct tm systime = *localtime(&t);

  // build the message
  sprintf(log, "[%d:%d:%d] %s \n", systime.tm_hour, systime.tm_min, systime.tm_sec, text);

  // The IO-block; only one can access log file at the same time
	int ret;
	while ((ret = write(fd, log, strlen(log))) == -1)
   	{
      // check if blocked,
   		// retry whenever needed
   		if (ret == EAGAIN || EWOULDBLOCK)
        {
   			continue;
   		}
   		else
        {
   			return;
   		}
   	}
}
