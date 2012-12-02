#include "ipcserverclient.h"
#include <signal.h>

#define USAGE_STRING "Invalid arguments.\nUsage: ./serverD.out new_server_key\n"
#define INVALID_SERVER_KEY "Invalid server key. Please specify a positive integer.\n"
#define MAX_CLIENTS 2

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
    //perror("msgrcv: Error while attempting to receive message...\n");
    //exit(EXIT_FAILURE);
  }
  else{
    //printf("Received %d bytes from message queue.\n", bytesRead);
    //printf("Message payload: Source: %ld\n", msgp->data.source);
    //printf("Message from %ld to %ld: %s\n", msgp->data.source, msgp->data.dest, msgp->data.msgstr);
  }
}

//sends a message to the client via the messsage queue
void send_message(char message[MSGSTR_LEN], int msgqid, long to, long from){
  msgbuf new_msg;
  new_msg.mtype = to; //reciever server
  data_st ds;
  ds.source = from;
  ds.dest = to; //receiver client
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


int main(int argc,char * argv[]){
  int qID;
  int key;
  int clients[MAX_CLIENTS];
  int current_num_clients = 0;
  int i;

  msgbuf client_msg_buffs[MAX_CLIENTS];
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

  printf("Server connected! Waiting for clients to connect...\n");
  printf("Connect client by running ./clientD.out %d new_client_key\n", key);

  while(1) {
    //reads a message from the message queue and prints to console
    int to;
    int from;
    char message[MSGSTR_LEN];
    msgbuf tempbuf;
    receive_message(qID, &tempbuf, key);

    to = tempbuf.data.dest;
    from = tempbuf.data.source;
    strncpy(message, tempbuf.data.msgstr, 1);
    if(current_num_clients == 0) {
      client_msg_buffs[0].data.dest = to;
      client_msg_buffs[0].data.source = from;
      strncpy(client_msg_buffs[0].data.msgstr, message, 1);
      clients[current_num_clients] = from;
      current_num_clients++;
    }
    else {
      int i = 0;
      int buff = -1;
      while(i < current_num_clients) {
        if(clients[i] == from) buff = i;
        i++;
      }
      if(buff != -1) {
        client_msg_buffs[buff].data.dest = to;
        client_msg_buffs[buff].data.source = from;
        strcat(client_msg_buffs[buff].data.msgstr, message);
        if(strcmp(message, "\0") == 0) {
          //printf("Message: %s\n", client_msg_buffs[buff].data.msgstr);
            if(client_msg_buffs[buff].data.dest == key) {
              if(strcmp(client_msg_buffs[buff].data.msgstr, CONNECT_MSG) == 0) {
                printf("Client %ld connected\n", client_msg_buffs[buff].data.source);
              }
              printf("Received message from %ld: %s\n",
                client_msg_buffs[buff].data.source,
                client_msg_buffs[buff].data.msgstr);
            }
            else {
              printf("Relaying message to %ld from %ld: %s\n",
                client_msg_buffs[buff].data.dest,
                client_msg_buffs[buff].data.source,
                client_msg_buffs[buff].data.msgstr);
              send_message(client_msg_buffs[buff].data.msgstr, qID, client_msg_buffs[buff].data.dest, client_msg_buffs[buff].data.source);
            }
          strcpy(client_msg_buffs[buff].data.msgstr, "");
        }
      }
      else {
        client_msg_buffs[current_num_clients].data.dest = to;
        client_msg_buffs[current_num_clients].data.source = from;
        strncpy(client_msg_buffs[current_num_clients].data.msgstr, message, 1);
        clients[current_num_clients] = from;
        current_num_clients++;
      }
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
