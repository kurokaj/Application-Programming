/*
 * Function defs for the hellolib
 */

#ifndef HH_HELLO_C
#define HH_HELLO_C

/*
- Param = id of the childprocess
- Prints greetings from this universe
- Returns negative integer if failed
*/
int hello(int world_id);

/*
- Param = id of the childprocess
- Prints farewells from this universe
- Returns negative integer if failed
*/
int byebye(int world_id);

#endif
