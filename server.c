#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



#define PERMISSIONS 0640

#define client1_mtype 1
#define client2_mtype 2L
#define MSGSTR_LEN 256

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
	if ((qID = msgget(key,IPC_CREAT | PERMISSIONS)) ==-1){
		perror("mssget: Error creating msg queue \n"); 
		exit(EXIT_FAILURE);
	}
	return qID;
}


//receives a message from the message queue and prints it to the console
void receive_message(int msgqid,msgbuf * msgp,long mtype){
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
void send_message(char message[],int msgqid,msgbuf * msgp,long mtype,long sourceID){
	data_st ds;
	ds.source = sourceID;

	//TODO: ensure size of message is not longer than MSGSTR_LEN-1
	strncpy(ds.msgstr,message,MSGSTR_LEN); //place message in server's local buffer
	// ds.msgstr[MSGSTR_LEN – 1] = '\0'; //for safety
	ds.msgstr[MSGSTR_LEN - 1] = '\0';
	msgp->mtype = mtype;
	msgp->data = ds;

	int ret = msgsnd(msgqid, &msgp, sizeof(data_st), 0);
	if (ret == -1) {
		perror("msgsnd: Error attempting to send message!"); 
		exit(EXIT_FAILURE);
	}

}

main(int argc,char * argv[]){
	int qID;
	// 1st commandline argument = key of message queue 
	if(argc < 2){
		qID = create_msg_queue(14);
	}else{
		//TODO: perform some type checking here
		qID = create_msg_queue(atoi(argv[1]));	
	}
	
	printf("Message queue created (id: %d)\n",qID);

	msgbuf localbuf;
	localbuf.mtype = client1_mtype;
	

	// while(1){ //server needs to keep runnning
		printf("Server: Waiting for client request...\n");
		int bytesRead = 0;
		receive_message(qID,&localbuf,client1_mtype); //reads a message from the message queue and prints to console
	// }
	
}
