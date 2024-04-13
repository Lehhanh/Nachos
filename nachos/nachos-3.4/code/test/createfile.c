#include "syscall.h"

int main(){
	int success;
	success = CreateFile("test.txt");
	PrintInt(success);
	Halt();
}