# Server/Client IPC (inter-process communications) written in C

## Written by Ryan Seys & Osazuwa Omigie (for Assignment 3 - SYSC 4001)

### Part B files

* clientB.c
* serverB.c

### Part C files

* clientC.c
* serverC.c
* ipcserverclient.h

### Part D files

* clientD.c
* serverD.c
* ipcserverclient.h

### Makefile

There is a make file included in this package, use it to build/compile the source code provided.
You can also use the Makefile to run the projects just run:

* For Part B (serverB.c)          $ make sb
* For Part B (clientB.c)          $ make cb

* For Part C (serverC.c)          $ make sc
* For Part C (clientC.c)          $ make cc
* For Part C (additional client)  $ ./clientC.out 42 123

* For Part D (serverD.c)          $ make sd
* For Part D (clientD.c)          $ make cd
* For Part D (additional client)  $ ./clientD.out 42 123

NOTE: Please notice that if you want to run an additional client, you must manually call the
command for it instead of relying on the Makefile to start it.

## Running Part B

Start Part B server:              $ make sb
  You will now have a server connected to message queue with key 42.
Start Part B client alongside:    $ make cb

Type text into the keyboard in the client. Then press enter when done typing.
This will send the message to the server (character-by-character), and once completed sending,
the message will be printed on the server, and then sent back to the client (again, character-by-character).

If you send "EXIT" as the message in the client to the server, it will quit the server and client.

## Running Part C

Start Part C server:              $ make sc
  You will now have a server connected to message queue with key 42.
Start Part C client alongside:    $ make cc
  You will now have a client connected with key 69, sending message to server 42.
Start another client alongside:    $ ./clientC.out 42 123
  You will now have a client connected with key 123, sending message to server 42.

To send a message, you must specify the recipient key (either server or client), and then the message
that you want to send to that recipient. There are some examples below.

e.g. from client 69 type:   42 hello there how are you?

  This will send the message "hello there how are you?" to the server (key=42),
  and will print it on the server because the message was for the server.


e.g. from client 69 type:   69 hello there how are you?

  This will send the message "hello there how are you?" to the server (key=42),
  and the server will then relay the message to client with key 69 (which in this
  case is the client we have currently running). Once client 69 recieves the message,
  it will print the message on the screen.


e.g. from client 69 type:   123 hello there how are you?

  This will send the message "hello there how are you?" to the server (key=42),
  and the server will then relay the message to client with key 123 (which is the
  other running server in another process). Once client 123 recieves the message,
  it will print the message on the screen.

## Running Part D

** NEW ** features:
 * Connect messages displayed on server when a client connects.
 * Disconnect messages displayed on server when a client disconnects.
 * Global (public) messages sent to all currently connected clients when no recipient key is specified.

Start Part D server:              $ make sd
  You will now have a server connected to message queue with key 42.
Start Part D client alongside:    $ make cd
  You will now have a client connected with key 69, sending message to server 42.
Start another client alongside:    $ ./clientD.out 42 123
  You will now have a client connected with key 123, sending message to server 42.

  ** NEW ** You will see a connect message for client 69 on the server once the client connects.

Start another client alongside:    $ ./clientD.out 42 123
  You will now have a client connected with key 123, sending message to server 42.

  ** NEW ** You will see a connect message for client 123 on the server once the client connects.


To send a message, you must specify the recipient key (either server or client), and then the message
that you want to send to that recipient. There are some examples below.

** NEW ** If you do not specify a recipient, it will go to everyone. This means that it will send the
message to the server and the server will relay it to every connected client.

** NEW** e.g. from client 69 type:   hey how are you?

  This will send the message "hey how are you?" to the server (key=42),
  and the server will relay it to every client it currently has listed as connected.
  This includes the sender, so the sender will recieve the message it sent to global.

e.g. from client 69 type:   42 hello there how are you?

  This will send the message "hello there how are you?" to the server (key=42),
  and will print it on the server because the message was for the server.

e.g. from client 69 type:   69 hello there how are you?

  This will send the message "hello there how are you?" to the server (key=42),
  and the server will then relay the message to client with key 69 (which in this
  case is the client we have currently running). Once client 69 recieves the message,
  it will print the message on the screen.

e.g. from client 69 type:   123 hello there how are you?

  This will send the message "hello there how are you?" to the server (key=42),
  and the server will then relay the message to client with key 123 (which is the
  other running server in another process). Once client 123 recieves the message,
  it will print the message on the screen.

** NEW ** Pressing CTRL-C or CTRL-Z in the client will send a "quit" signal to the terminal,
and this signal will be caught and used to send disconnect messages to the server, before quitting.
