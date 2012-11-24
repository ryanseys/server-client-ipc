#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>


#define PERMISSIONS 0640

main(){
	//printf("server started");

	//create a message queue
	int qID = msgget(IPC_PRIVATE,IPC_CREAT | PERMISSIONS);
	printf("message queue id: %d\n",qID);

}
