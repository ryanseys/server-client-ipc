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
