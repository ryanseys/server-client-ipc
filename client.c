#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define PERMISSIONS 0640
#define client1_mtype 1
#define client2_mtype 2L
#define MSGSTR_LEN 256
#define NUM_THREADS 2

typedef struct data_st{
  long source;
  char msgstr[MSGSTR_LEN];
} data_st;

typedef struct msgbuf_st {
   long mtype; /* A message type > 0. */
   data_st data; /* Data */
} msgbuf;

//receives a message from the message queue and prints it to the console
void receive_message(int msgqid, msgbuf * msgp, long mtype){
  int bytesRead = msgrcv(msgqid,msgp,sizeof(struct data_st),mtype,0);
  if (bytesRead == -1) {
    if (errno == EIDRM) {
      fprintf(stderr, "Message queue removed while waiting!\n");
    }
    perror("msgrcv: Error while attempting to receive message...\n");
    exit(EXIT_FAILURE);
  }
  else{
    printf("Received %d bytes from message queue.\n", bytesRead);
    printf("Message payload: Source: %ld\n", msgp->data.source);
    printf("Message payload: Message string: %s\n", msgp->data.msgstr);
  }
}

//sends a message to the client via the messsage queue
void send_message(char message[], int msgqid, long to, long from){
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

void * send_thread(void * arg) {
  int * val = arg;
  char buffer[MSGSTR_LEN];
  printf("In the send thread with arg %d.\n", *val);

  printf("Enter message to send: ");
  while(fgets(buffer, MSGSTR_LEN, stdin)) {
    printf("Sending %s\n", buffer);
    send_message(buffer, *val, 14, 1);
    printf("Enter message to send: ");
  }

  sleep(5);
  int * myretp = malloc(sizeof(int));
  if (myretp == NULL) {
    perror("malloc error");
    pthread_exit(NULL);
  }
  *myretp = (*val);
  return myretp; /* Same as: pthread_exit(myretp); */
}

void * receive_thread(void * arg) {
  int * val = arg;
  printf("In the receive thread with arg %d.\n", *val);
  sleep(2);
  int * myretp = malloc(sizeof(int));
  if (myretp == NULL) {
    perror("malloc error");
    pthread_exit(NULL);
  }
  *myretp = (*val);
  return myretp; /* Same as: pthread_exit(myretp); */
}

int main(int argc, char * argv[]) {
  pthread_t threads[NUM_THREADS];
  int targs[NUM_THREADS];
  int i, ret, ret2;

  int qID;
  int key;
  // 1st commandline argument = key of message queue
  if(argc == 2) { // get command line argument
    key = atoi(argv[1]);
    printf("Trying to get queue (key: %d)\n", key);
    qID = msgget(key, 0); // 0 for making use of existing queue
  }
  else if(argc == 1) { //assume 14
    key = 14;
    printf("Trying to get queue (key: %d)\n", key);
    qID = msgget(key, 0);
  }
  else {
    //TODO: perform some type checking here
    printf("Too many arguments: %d\n", argc);
    exit(-1);
  }

  if(qID < 0) {
    printf("Failed to get queue, got status: %d\n", qID);
    exit(-1);
  }

  printf("Message queue got (key: %d)\n", key);

  //create sender thread
  targs[0] = qID;
  ret = pthread_create(&(threads[0]), NULL, send_thread, &(targs[0]));
  if (ret == -1) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  targs[1] = 1;
  ret = pthread_create(&(threads[1]), NULL, receive_thread, &(targs[1]));
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

/*
  int i;
  for(i = 1; i <= 5; i++) {
    char string[MSGSTR_LEN];
    snprintf(string, MSGSTR_LEN-1, "%d: hello server.", i);
    send_message(string, qID, key, 1);
    printf("Sent hello server to %d\n", key);
    sleep(1);
  }

  msgbuf localbuf;
  localbuf.mtype = client1_mtype;
  int n = 5;
  while(n != 0) {
    receive_message(qID, &localbuf, 1);
    n--;
  }
  */
}

