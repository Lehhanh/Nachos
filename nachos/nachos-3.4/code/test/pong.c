#include "syscall.h"

int main()
{
	int i;	
	for(i = 0; i < 1000; i++)
	{
		Down("s2");
		PrintChar('B');
		Up("s1");
	}
}
