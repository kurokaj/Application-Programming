#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "hellolib.h"


int hello(int world_id)
{
    // print "hello from world X"
    printf("Hello from world %d.\n", world_id);

    return 1;
}

int byebye(int world_id)
{
    // print "hello from world X"
    printf("Byebye from world %d.\n", world_id);

    return 1;
}
