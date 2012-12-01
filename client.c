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
#define CLIENT_2_KEY 70
#define MSGSTR_LEN 256
#define NUM_THREADS 2

typedef struct data_st{
  long source; //source number
  long dest; //destination number
  char msgstr[MSGSTR_LEN];
} data_st;

typedef struct msgbuf_st {
   long mtype; /* A message type > 0. */
   data_st data; /* Data */
} msgbuf;

//receives a message from the message queue and prints it to the console
void receive_message(int msgqid, msgbuf * msgp, long mtype) {
  int bytesRead = msgrcv(msgqid,msgp,sizeof(struct data_st),mtype,IPC_NOWAIT);
  if (bytesRead == -1) {
    if (errno == EIDRM) {
      fprintf(stderr, "Message queue removed while waiting!\n");
    }
    //perror("msgrcv: Error while attempting to receive message...\n");
    //exit(EXIT_FAILURE);
  }
  else{
    //printf("Received %d bytes from message queue.\n", bytesRead);
    //printf("Message payload: Source: %ld\n", msgp->data.source);
    //printf("Message payload: Message string: %s\n", msgp->data.msgstr);
  }
}

//sends a message to the client via the messsage queue
void send_message(char message[], int msgqid, long to, long from, long to_client){
  msgbuf new_msg;
  new_msg.mtype = to; //reciever
  data_st ds;
  ds.source = from;
  ds.dest = to_client;
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
  int * qID = arg;
  int * key = arg+sizeof(int);
  int * client_key = arg+sizeof(int)*2;
  int other_client_key;

  char buffer[MSGSTR_LEN];
  //printf("In the send thread with arg %d.\n", *val);

  while(fgets(buffer, MSGSTR_LEN, stdin)) {
    if (buffer[strlen(buffer) - 1] == '\n') {
      buffer[strlen(buffer) - 1] = '\0';
    }

    char* input = buffer;
    char* space = " ";
    int start = 0;
    int pos;
    char numberbuff[255];
    char messagebuff[255];
    pos = strcspn(input, space);
    if(pos != NULL) {
      other_client_key = atoi(strncpy(numberbuff, input, pos));
      if(other_client_key != NULL) {
        char * message = buffer+pos+1;
        printf("Sending %s to %d\n", message, other_client_key);
        send_message(message, *qID, *key, *client_key, other_client_key);
        if(strcmp(message, "exit") == 0) exit(0); //exit if you say to exit
      }
      else printf("Usage: receiving_client_key message_string\n");
    }
    else printf("Usage: receiving_client_key message_string\n");
  }

  int * myretp = malloc(sizeof(int));
  if (myretp == NULL) {
    perror("malloc error");
    pthread_exit(NULL);
  }
  *myretp = (*qID);
  return myretp; /* Same as: pthread_exit(myretp); */
}

void * receive_thread(void * arg) {
  int * qID = arg;
  int * key = arg+sizeof(int);
  int * client_key = arg+sizeof(int)*2;
  //printf("In the receive thread with arg %d.\n", *val);

  msgbuf localbuf;
  localbuf.mtype = *client_key;
  while(1) {
    receive_message(*qID, &localbuf, *client_key);
    if(strcmp(localbuf.data.msgstr, "") == 0) {
      sleep(1);
    }
    else {
      printf("Received message from server: %s\n", localbuf.data.msgstr);
      strncpy(localbuf.data.msgstr, "", MSGSTR_LEN);
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

void start_thread(pthread_t * thread) {
  void * thread_ret_ptr;
  int ret;
  ret = pthread_join(*thread, &thread_ret_ptr);
  if (ret == -1) { perror("Thread join error");
    exit(EXIT_FAILURE);
  }
  if (thread_ret_ptr != NULL) {
    int * intp = (int *) thread_ret_ptr;
    printf("Receive thread returned: %d\n", *intp);
    free(thread_ret_ptr);
  }

}

void create_thread(pthread_t * thread, void * vars, int thread_type) {
  int ret;
  if(thread_type == 1) ret = pthread_create(thread, NULL, send_thread, vars);
  if(thread_type == 2) ret = pthread_create(thread, NULL, receive_thread, vars);
  if (ret == -1) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char * argv[]) {
  pthread_t threads[NUM_THREADS];
  int targs[NUM_THREADS];
  int i, ret, ret2;
  int qID;
  int key;
  int client_key;
  int vars[2];
  char * input;
  // 1st commandline argument = key of message queue
  if(argc == 3) { // get command line argument
    key = atoi(argv[1]);
    client_key = atoi(argv[2]);
    //printf("Trying to get queue (key: %d)\n", key);
    qID = msgget(key, 0); // 0 for making use of existing queue
  }
  else {
    printf("Invalid arguments.\nUsage: ./client running_server_key new_client_key\n");
    exit(-1);
  }

  if(qID < 0) {
    printf("Failed to get queue, got status: %d\n", qID);
    exit(-1);
  }

  // vars = [qID, key]
  vars[0] = qID;
  vars[1] = key;
  vars[2] = client_key;
  //printf("Message queue got (key: %d)\n", key);

  //create sender thread
  create_thread(&(threads[0]), vars, 1);
  create_thread(&(threads[1]), vars, 2);

  start_thread(&(threads[0]));
  start_thread(&(threads[1]));

  return 0;
}
