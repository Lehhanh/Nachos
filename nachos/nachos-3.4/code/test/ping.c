#include "syscall.h"

int main()
{
	int i;
	for(i = 0; i < 1000; i++)
	{
		Down("s1");
		PrintChar('A');
		Up("s2");
	}
}
