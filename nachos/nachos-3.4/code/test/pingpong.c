#include "syscall.h"

int main()
{
	int success;
	int pingPID, pongPID;
	PrintString("Ping-Pong test starting...\n\n");
	success = CreateSemaphore("s1", 1);
	if (success == -1){
		return 1;
	}
	success = CreateSemaphore("s2", 0);
	if (success == -1){
		return 1;
	}
	pingPID = Exec("./test/ping");
	pongPID = Exec("./test/pong");

	Join(pingPID);
	Join(pongPID);
	PrintString("\n\n");
	Halt();
}

