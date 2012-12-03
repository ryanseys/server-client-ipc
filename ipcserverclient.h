#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define PERMISSIONS 0640
#define MSGSTR_LEN 256
#define CONNECT_MSG "::CONNECT::"
#define DISCONNECT_MSG "::DISCONNECT::"
#define EXIT_STR "EXIT"

/**
 * Data for message struct
 */
typedef struct data_st{
  long source; //source number
  long dest; //destination number
  char msgstr[MSGSTR_LEN];
} data_st;

/**
 * Message buffer structure
 */
typedef struct msgbuf_st {
   long mtype; /* A message type > 0. */
   data_st data; /* Data */
} msgbuf;
