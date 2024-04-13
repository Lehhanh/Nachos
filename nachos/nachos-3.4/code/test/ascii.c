#include "syscall.h"

int main()
{
	int i, id, cnt, temp;
	char buffer[1000];
	PrintString("Bang ma ASCII la: \n");
	id = Open("ascii.txt", 0);
	cnt = 0;
	for (i = 32; i < 127; i++)
	{
		buffer[cnt++] = i;
		buffer[cnt++] = ' ';
	}
	PrintString(buffer);
	Write(buffer, cnt, id);
	Close(id);
	PrintString("\n");
	Halt();
	return 0;
}