#include "ipcserverclient.h"

#define NUM_THREADS 2
#define USAGE_STRING "Usage: receiving_client_key message_to_send (Example: 42 hello )\n"
#define INIT_USAGE "Invalid arguments.\nUsage: ./clientD.out running_server_key new_client_key\n"

//receives a message from the message queue and prints it to the console
void receive_message(int msgqid, msgbuf * msgp, long mtype) {
  int bytesRead = msgrcv(msgqid, msgp, sizeof(struct data_st), mtype, 0);
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

//sends a message to the server via the messsage queue
void send_message(char message[MSGSTR_LEN], int msgqid, long to, long from, long to_client){
  msgbuf new_msg;
  new_msg.mtype = to; //reciever server
  data_st ds;
  ds.source = from;
  ds.dest = to_client; //receiver client
  char * null = "\0";
  int length = strlen(message);
  if(MSGSTR_LEN < length) length = MSGSTR_LEN;
  int i;
  //send a character at a time
  for(i = 0; i < length; i++) {
    strncpy(ds.msgstr, &(message[i]), 1);
    new_msg.data = ds;
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

void * send_thread(void * arg) {
  int * qID = arg;
  int * key = arg+sizeof(int);
  int * client_key = arg+sizeof(int)*2;
  int other_client_key;

  send_message(CONNECT_MSG, *qID, *key, *client_key, *key);
  printf("You are now connected as client %d\n%s", *client_key, USAGE_STRING);

  char buffer[MSGSTR_LEN];

  while(fgets(buffer, MSGSTR_LEN, stdin)) {
    if (buffer[strlen(buffer) - 1] == '\n') {
      buffer[strlen(buffer) - 1] = '\0';
    }

    char * input = buffer;
    char * space = " ";
    int start = 0;
    int pos;
    char numberbuff[255];
    char messagebuff[255];
    pos = strcspn(input, space);
    if(pos) {
      other_client_key = atoi(strncpy(numberbuff, input, pos));
      if(other_client_key) {
        char * message = buffer+pos+1;
        printf("Sending \"%s\" to %d\n", message, other_client_key);
        send_message(message, *qID, *key, *client_key, other_client_key);
        if(strcmp(message, "exit") == 0) exit(0); //exit if you say to exit
      }
      else printf("%s%s", "Invalid input!\n", USAGE_STRING);
    }
    else printf(USAGE_STRING);
    strncpy(numberbuff, "", 255); //clear buffer
    strncpy(messagebuff, "", 255); //clear buffer
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

  int to;
  int from;
  char message[MSGSTR_LEN];
  msgbuf tempbuf;
  tempbuf.mtype = *client_key;

  msgbuf messagebuf;
  while(1) {
    receive_message(*qID, &tempbuf, *client_key);

    to = tempbuf.data.dest;
    from = tempbuf.data.source;
    strncpy(message, tempbuf.data.msgstr, 1);

    messagebuf.data.dest = to;
    messagebuf.data.source = from;
    strcat(messagebuf.data.msgstr, message);
    if(strcmp(message, "\0") == 0) {
      printf("Received message from %ld: \"%s\"\n", messagebuf.data.source, messagebuf.data.msgstr);
      strncpy(messagebuf.data.msgstr, "", MSGSTR_LEN);
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

void printlol() {

}

int main(int argc, char * argv[]) {

// a definition mentioning inline


  pthread_t threads[NUM_THREADS];
  int targs[NUM_THREADS];
  int i, ret, ret2;
  int qID;
  int key;
  int client_key;
  int vars[3];
  char * input;

  inline void exiting() {
    send_message(DISCONNECT_MSG, qID, key, client_key, key);
    exit(0);
  }

  signal(SIGINT, exiting);
  signal(SIGTSTP, exiting);

  // 1st commandline argument = key of message queue
  if(argc == 3) { // get command line argument
    key = atoi(argv[1]);
    client_key = atoi(argv[2]);
    if(!key || !client_key) {
      printf(INIT_USAGE);
      exit(-1);
    }
    //printf("Trying to get queue (key: %d)\n", key);
    qID = msgget(key, 0); // 0 for making use of existing queue
  }
  else {
    printf(INIT_USAGE);
    exit(-1);
  }

  if(qID < 0) {
    printf("Failed to get queue, got status: %d\n", qID);
    exit(-1);
  }

  // vars = [qID, key, client_key]
  vars[0] = qID;
  vars[1] = key;
  vars[2] = client_key;

  //create sender thread
  create_thread(&(threads[0]), vars, 1);
  create_thread(&(threads[1]), vars, 2);

  start_thread(&(threads[0]));
  start_thread(&(threads[1]));
  return 0;
}
