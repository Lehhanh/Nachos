#include "syscall.h"
int main(){
	char str[100];
	int length;
	length = 20;
	ReadString(str, length);
	PrintString(str);
	Halt();
}	