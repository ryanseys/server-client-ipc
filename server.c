#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


int create_msg_queue(){
	return msgget(IPC_PRIVATE,IPC_CREAT | PERMISSIONS);
}


ssize_t receive_message(int msgqid,msgbuf * msgp,long mtype){
	return msgrcv(msgqid,msgp,sizeof(struct data_st),mtype,0);
}


main(){

	int qID = create_msg_queue();
	printf("message queue id: %d\n",qID);
	char * str = "hello";

	data_st ds;
	ds->source = client1_mtype;
	strncpy(ds->msgstr,"hello",MSGSTR_LEN);
	//memcpy(ds->msgstr,str,256);
	//ds->msgstr = str;

	msgbuf * mbuf;
	mbuf->mtype = client1_mtype;

	mbuf->data = ds;

	receive_message(qID,&mbuf,client1_mtype);

	printf("should not be here");

}
