#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


main(){
	printf("server started");

	//create a message queue
	int qID = msgget(123,IPC_PRIVATE);
	printf("queue id: %i",qID);
}
