#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define PERMISSIONS 0640
#define DEFAULT_SERVER_KEY 42
#define CLIENT_KEY 69
#define MSGSTR_LEN 256
#define NUM_THREADS 2
#define EXIT_STRING "EXIT"

/**
 * Data for message struct
 */
typedef struct data_st{
  long source;
  char msgstr[MSGSTR_LEN];
} data_st;

/**
 * Message buffer structure
 */
typedef struct msgbuf_st {
   long mtype; /* A message type > 0. */
   data_st data; /* Data */
} msgbuf;

/**
 * Receives a message from the message queue and puts it in a buffer
 * @param msgqid Queue key
 * @param msgp   pointer to message buffer
 * @param mtype  Type of message to recieve
 */
void receive_message(int msgqid, msgbuf * msgp, long mtype) {
  //blocking receive
  int bytesRead = msgrcv(msgqid,msgp,sizeof(struct data_st),mtype,0);
  if (bytesRead == -1) {
    if (errno == EIDRM) {
      fprintf(stderr, "Message queue removed while waiting!\n");
    }
  }
}

/**
 * Sends a message to the server via the messsage queue
 * @param message Message to send
 * @param msgqid  Queue key
 * @param to      server key to send to
 * @param from    client/server key sent from
 */
void send_message_old(char message[], int msgqid, long to, long from){
  msgbuf new_msg;
  new_msg.mtype = to; //reciever
  data_st ds;
  ds.source = from;
  strncpy(ds.msgstr,message,MSGSTR_LEN);
  ds.msgstr[MSGSTR_LEN - 1] = '\0';
  new_msg.data = ds;

  int ret = msgsnd(msgqid, (void *) &new_msg, sizeof(data_st), IPC_NOWAIT);
  if (ret == -1) {
    perror("msgsnd: Error attempting to send message!");
    exit(EXIT_FAILURE);
  }
}

/**
 * Sends a message to the server via the messsage queue
 * @param message Message to send
 * @param msgqid  Queue key
 * @param to      server key to send to
 * @param from    client/server key sent from
 */
void send_message(char message[MSGSTR_LEN], int msgqid, long to, long from){
  msgbuf new_msg;
  new_msg.mtype = to; //reciever server
  data_st ds;
  ds.source = from;
  //ds.dest = to_client; //receiver client
  char * null = "\0";
  int length = strlen(message);
  if(MSGSTR_LEN < length) length = MSGSTR_LEN;
  int i;
  //send a character at a time
  for(i = 0; i < length; i++) {
    strncpy(ds.msgstr, &(message[i]), 1);
    new_msg.data = ds;
    //blocking send to prevent error
    int ret = msgsnd(msgqid, (void *) &new_msg, sizeof(data_st), 0);
    if (ret == -1) {
      perror("msgsnd: Error attempting to send message!");
      exit(EXIT_FAILURE);
    }
  }
  strncpy(ds.msgstr, null, 1);
  new_msg.data = ds;

  int ret = msgsnd(msgqid, (void *) &new_msg, sizeof(data_st), 0);
  if (ret == -1) {
    perror("msgsnd: Error attempting to send message!");
    exit(EXIT_FAILURE);
  }
}

/**
 * Thread for running the sending of messages.
 * @param arg Arguments needed to send messages
 */
void * send_thread(void * arg) {
  int * qID = arg;
  int * key = arg+sizeof(int);
  char buffer[MSGSTR_LEN];

  //continue to get user input for sending messages
  while(fgets(buffer, MSGSTR_LEN, stdin)) {
    if (buffer[strlen(buffer) - 1] == '\n') {
      buffer[strlen(buffer) - 1] = '\0';
    }
    printf("Sending \"%s\"\n", buffer);
    send_message(buffer, *qID, *key, CLIENT_KEY);
    if(strcmp(buffer, EXIT_STRING) == 0) exit(0); //exit if you say to exit
  }

  int * myretp = malloc(sizeof(int));
  if (myretp == NULL) {
    perror("malloc error");
    pthread_exit(NULL);
  }
  *myretp = (*qID);
  return myretp; /* Same as: pthread_exit(myretp); */
}

/**
 * Thread that receives the messages
 * @param arg Arguments needed to receive messages from the queue
 */
void * receive_thread(void * arg) {
  int * qID = arg;
  int * key = arg+sizeof(int);

  int to;
  int from;
  char message[MSGSTR_LEN];
  msgbuf tempbuf;
  msgbuf localbuf;
  localbuf.mtype = CLIENT_KEY;

  while(1) {
    receive_message(*qID, &tempbuf, CLIENT_KEY);
    strncpy(message, tempbuf.data.msgstr, 1);
    strcat(localbuf.data.msgstr, message);
    if(strcmp(message,"\0") == 0){
      printf("Received message from server: \"%s\"\n", localbuf.data.msgstr);
      strcpy(localbuf.data.msgstr,""); //clean up local buffer

    }
  }

  int * myretp = malloc(sizeof(int));
  if (myretp == NULL) {
    perror("malloc error");
    pthread_exit(NULL);
  }
  *myretp = (*qID);
  return myretp; /* Same as: pthread_exit(myretp); */
}

/**
 * Main program to run the sending
 * of messages and receival of messages
 * @param  argc Argument count
 * @param  argv Argument array
 * @return      Exit code
 */
int main(int argc, char * argv[]) {
  pthread_t threads[NUM_THREADS];
  int targs[NUM_THREADS];
  int i, ret, ret2;
  int qID;
  int key;
  int vars[2];

  // 1st commandline argument = key of message queue
  if(argc == 2) { // get command line argument
    key = atoi(argv[1]);
    qID = msgget(key, 0); // 0 for making use of existing queue
  }
  else if(argc == 1) { //assume 42
    key = DEFAULT_SERVER_KEY;
    qID = msgget(key, 0);
  }
  else {
    printf("Too many arguments: %d\n", argc);
    exit(-1);
  }

  if(qID < 0) {
    printf("Failed to get queue, got status: %d\n", qID);
    exit(-1);
  }

  vars[0] = qID;
  vars[1] = key;

  //create sender thread
  ret = pthread_create(&(threads[0]), NULL, send_thread, &(vars));
  if (ret == -1) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  //create receive thread
  ret = pthread_create(&(threads[1]), NULL, receive_thread, &(vars));
  if (ret == -1) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  /* Wait for the send thread to end. */
  void * thread_ret_ptr_send;
  ret = pthread_join(threads[0], &thread_ret_ptr_send);
  if (ret == -1) { perror("Thread join error");
    exit(EXIT_FAILURE);
  }
  if (thread_ret_ptr_send != NULL) {
    int * intp = (int *) thread_ret_ptr_send;
    printf("Send thread returned: %d\n", *intp);
    free(thread_ret_ptr_send);
  }

  /* Wait for the receive thread to end. */
  void * thread_ret_ptr_receive;
  ret2 = pthread_join(threads[1], &thread_ret_ptr_receive);
  if (ret2 == -1) { perror("Thread join error");
    exit(EXIT_FAILURE);
  }
  if (thread_ret_ptr_receive != NULL) {
    int * intp = (int *) thread_ret_ptr_receive;
    printf("Receive thread returned: %d\n", *intp);
    free(thread_ret_ptr_receive);
  }

  return 0;
}
