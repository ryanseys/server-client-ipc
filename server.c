#include "ipcserverclient.h"

#define USAGE_STRING "Invalid arguments.\nUsage: ./server.out new_server_key\n"
#define INVALID_SERVER_KEY "Invalid server key. Please specify a positive integer.\n"

int create_msg_queue(int key){
	int qID;
	// printf("%d\n",key);
	if ((qID = msgget(key, IPC_CREAT | PERMISSIONS)) == -1){
		perror("mssget: Error creating msg queue \n");
		exit(EXIT_FAILURE);
	}
	return qID;
}

//receives a message from the message queue and prints it to the console
void receive_message(int msgqid, msgbuf * msgp, long mtype){
	int bytesRead = msgrcv(msgqid, msgp, sizeof(struct data_st), mtype, 0);
	if (bytesRead == -1) {
		if (errno == EIDRM) {
			fprintf(stderr, "Message queue removed while waiting!\n");
		}
		perror("msgrcv: Error while attempting to receive message...\n");
		exit(EXIT_FAILURE);
	}
	else{
		//printf("Received %d bytes from message queue.\n", bytesRead);
		//printf("Message payload: Source: %ld\n", msgp->data.source);
		//printf("Message from %ld to %ld: %s\n", msgp->data.source, msgp->data.dest, msgp->data.msgstr);
	}
}

//sends a message to the client via the messsage queue
void send_message(char message[], int msgqid, long to, long from){
  msgbuf new_msg;
  new_msg.mtype = to; //reciever
  data_st ds;
  ds.source = from;
  ds.dest = to;
  strncpy(ds.msgstr,message,MSGSTR_LEN);
  ds.msgstr[MSGSTR_LEN - 1] = '\0';
  new_msg.data = ds;

  int ret = msgsnd(msgqid, (void *) &new_msg, sizeof(data_st), IPC_NOWAIT);
  if (ret == -1) {
    perror("msgsnd: Error attempting to send message!");
    exit(EXIT_FAILURE);
  }
}

int main(int argc,char * argv[]){
	int qID;
	int key;
	// 1st commandline argument = key of message queue
	if(argc == 2){
    key = atoi(argv[1]);
    if(key <= 0) {
      printf(INVALID_SERVER_KEY);
      exit(-1);
    }
    qID = create_msg_queue(key);
	}
  else {
    printf(USAGE_STRING);
    exit(-1);
  }

	if(qID < 0) {
		printf("Failed to create queue.\n");
		exit(-1); //failed
	}

	msgbuf localbuf_client1;
  msgbuf localbuf_client2;

  printf("Server connected! Waiting for clients to connect...\n");
  printf("Connect client by running ./client.out %d new_client_key\n", key);

	while((strcmp(localbuf_client1.data.msgstr, "exit") != 0) &&
        (strcmp(localbuf_client2.data.msgstr, "exit") != 0)) {
    //reads a message from the message queue and prints to console
		receive_message(qID, &localbuf_client1, key);
    int to = localbuf_client1.data.dest;
    int from = localbuf_client1.data.source;
    //if the message is for the server
    if(to == key) {
      printf("Received message from %d: %s\n", from, localbuf_client1.data.msgstr);
    }
    else {
      printf("Relaying message to %d\n", to);
      send_message(localbuf_client1.data.msgstr, qID, to, from);
    }
	}

  /* Assuming that msqid has been obtained beforehand. */
  if (msgctl(qID, IPC_RMID, NULL) == -1) {
    if (errno == EIDRM) {
      fprintf(stderr, "Message queue already removed.\n");
    }
    else {
      perror("Error while removing message queue");
    }
  }
  return 0;
}
