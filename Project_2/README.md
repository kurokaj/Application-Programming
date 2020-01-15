The Data Transfer App.

Run:
The app can be compiled with command "make" and run with "./main". This opens the server that runs until the user presses ctrl+c. The client processes can be run by ./transmitter "path to file". The make file also compiles tester that spawns 20 child transmitters. The tester can be run with ./tester.

Actions:
The server opens three pipes in order to communicate with the clients. Clients can contact the server any time and are served in series (could not get the parallel working). The client reads text from the file given as parameter after receiving info that it is served. The server receives the text through FIFO-pipe and writes it to a new output-file.  

Log:
The server program prints its actions to a .log file with a external function. The log function is generated in own library.

Library Log:
I created a own library for the log writing process.

Signal:
The user can terminate the program with ctrl+c. It terminates all child processes and the main process in all of the executables.
