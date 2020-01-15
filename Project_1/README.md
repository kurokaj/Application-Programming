The Helloworld app.

Run:
The app can be compiled with command "make" and run with "./main".

Actions:
The app reads instructions from config.-file "travelplan.txt" and opens up X amount of child processes (new worlds) mentioned in travelplan. Then every child process prints greetings and farewells from that world and exits. The main program exits when every child process has returned.

Log:
The program prints its actions to a .log file with a external function. This is created with non-blocking I/O.

Library hellolib:
I created a own library for the greeting and farewell process.

Signal:
The user can terminate the program with ctrl+c. It terminates all child processes and the main process.
