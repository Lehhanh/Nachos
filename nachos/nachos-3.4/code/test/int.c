#include "syscall.h"
int main(){
	int result;
	result = ReadInt();
	PrintInt(result);
	Halt();
}