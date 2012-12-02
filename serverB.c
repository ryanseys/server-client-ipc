#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PERMISSIONS 0640
#define MSGSTR_LEN 256
#define DEFAULT_SERVER_KEY 42

typedef struct data_st{
	long source;
	char msgstr[MSGSTR_LEN];
} data_st;

typedef struct msgbuf_st {
   long mtype; /* A message type > 0. */
   data_st data; /* Data */
} msgbuf;

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
int receive_message(int msgqid, msgbuf * msgp, long mtype){
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
		printf("Message from %ld: %s\n", msgp->data.source, msgp->data.msgstr);
    return msgp->data.source;
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

int main(int argc,char * argv[]){
	int qID;
	int key;
	// 1st commandline argument = key of message queue
	if(argc < 2){
		key = DEFAULT_SERVER_KEY;
		qID = create_msg_queue(key);
	} else {
		//TODO: perform some type checking here
		key = atoi(argv[1]);
		qID = create_msg_queue(key);
	}

	if(qID < 0) {
		printf("Failed to create queue.\n");
		exit(-1); //failed
	}
	else {
		printf("Creating a queue with key: %d\n", key);
	}

	msgbuf localbuf_client1;
  msgbuf localbuf_client2;

  printf("Server connected! Waiting for client to connect...\n");
  printf("Connect client by running ./client %d\n", key);

	while((strcmp(localbuf_client1.data.msgstr, "exit") != 0) && (strcmp(localbuf_client1.data.msgstr, "exit") != 0)){
		//printf("Send messages to mtype: %d\n", key);
		int sender = receive_message(qID, &localbuf_client1, key); //reads a message from the message queue and prints to console
    printf("Relaying message to %d\n", sender);
    send_message(localbuf_client1.data.msgstr, qID, sender, key);
	}

  /* Assuming that msqid has been obtained beforehand. */
  if (msgctl(qID, IPC_RMID, NULL) == -1) {
  /* As an example for checking errno. */
    if (errno == EIDRM) {
      fprintf(stderr, "Message queue already removed.\n");
    }
    else {
      perror("Error while removing message queue");
    }
  }
  return 0;
}
