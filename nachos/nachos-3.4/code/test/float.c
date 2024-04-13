#include "syscall.h"
int main(){
	float f;
	ReadFloat(&f);
	PrintFloat(&f);
	Halt();
}