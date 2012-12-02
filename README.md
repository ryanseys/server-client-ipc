Assignment 3 for SYSC 4001
==========================

## Written by Ryan Seys & Osazuwa Omigie

Part B files
============

* clientB.c
* serverB.c

Part C files
============

* clientC.c
* serverC.c
* ipcserverclient.h

Part D files
============

* clientD.c
* serverD.c
* ipcserverclient.h

Makefile
========

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

Running Part B
==============

Start Part B server:              $ make sb
Start Part B client alongside:    $ make cb

Type text into the keyboard in the client. Then press enter when done typing.
This will send the message to the server (character-by-character), and once completed sending,
the message will be printed on the server, and then sent back to the client (again, character-by-character).

If you send "EXIT" as the message in the client to the server, it will quit the server and client.

Running Part C
==============

Start Part C server:              $ make sc
Start Part C client alongside:    $ make cc
