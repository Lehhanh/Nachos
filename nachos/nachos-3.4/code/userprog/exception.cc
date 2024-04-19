// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <cstring>
#include <cmath>
#define READ_WRITE 0
#define READ_ONLY 1
#define STDIN 2
#define STDOUT 3

#define MAXLENGTH 100

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void IncrementPC(){
	int valuePC;
	valuePC = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, valuePC);
	valuePC = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, valuePC);
	machine->WriteRegister(NextPCReg, valuePC + 4);
}

char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf,0,limit+1);
	for (i = 0 ; i < limit ;i++)
	{
		machine->ReadMem(virtAddr+i,1,&oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr,int len,char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar= (int) buffer[i];
		machine->WriteMem(virtAddr+i,1,oneChar);
		i++;
	}while(i < len && oneChar != 0);
	return i;
}

void SC_Sub_Handler(){
	int op1, op2, result;
	op1 = machine->ReadRegister (4);
	op2 = machine->ReadRegister (5);
	result = op1 - op2;
	machine->WriteRegister(2, result);
}

void SC_ReadInt_Handler(){
	char* buffer;
	int numbytes, number, i;
	bool isNegative;
	buffer = new char[MAXLENGTH];
	numbytes = gSynchConsole->Read(buffer, MAXLENGTH);//doc cac ky tu duoc nhap tren console vao buffer
	//doc console bi loi
	if (numbytes == -1){
		machine->WriteRegister(2, 0);//ghi ket qua vao thanh ghi so 2
		delete[] buffer;
		return;
	}
	number = 0;  //ket qua tra ve
	i = 0;       //chi so cua buffer
	isNegative = false;
	//kiem tra so am
	if (buffer[0] == '-'){
		isNegative = true;
		i++;
	}
	//kiem tra lan luot cac ky tu co phai chu so khong
	//neu la chu so thi them vao ket qua tra ve
	//neu khong la chu so thi break vong lap va ket qua tra ve la 0
	for (;i < numbytes;i++){
		if (buffer[i] < '0' || buffer[i] > '9'){
			DEBUG('a', "Invalid integer number.\n");
			number = 0;
			break;

		}
		else{
			number = number * 10 + (int)(buffer[i] - '0');
		}
	}
	if (isNegative){
		number = -number;
	}
	machine->WriteRegister(2, number);//ghi ket qua vao thanh ghi so 2
	delete[] buffer;
}

void SC_PrintInt_Handler(){
	int number, numbytes, firstNumIndex, temp, i;
	bool isNegative;
	char* buffer;
	number = machine->ReadRegister(4);
	if(number == 0)
    {
        gSynchConsole->Write("0", 1); // In ra man hinh so 0
        return;    
    }
    
    /*Qua trinh chuyen so thanh chuoi de in ra man hinh*/
    isNegative = false; // gia su la so duong
    numbytes = 0; // Bien de luu so chu so cua number
    firstNumIndex = 0; 

    if(number < 0)
    {
        isNegative = true;
        number = number * -1; // Nham chuyen so am thanh so duong de tinh so chu so
        firstNumIndex = 1; 
    } 	
    
    temp = number; // bien tam cho number
    while(temp)
    {
        numbytes++;
        temp /= 10;
    }

// Tao buffer chuoi de in ra man hinh
    buffer = new char[MAXLENGTH];
    i = firstNumIndex + numbytes - 1;
    for(; i >= firstNumIndex; i--)
    {
        buffer[i] = (char)((number % 10) + 48);
        number /= 10;
    }
    if(isNegative)
    {
        buffer[0] = '-';
		buffer[numbytes + 1] = 0;
        gSynchConsole->Write(buffer, numbytes + 1);
        delete[] buffer;
        return;
    }
	buffer[numbytes] = 0;	
    gSynchConsole->Write(buffer, numbytes);
    delete[] buffer;
}

void SC_ReadFloat_Handler(){
	int virtAddr, numbytes, i;
	char* buffer;
	float* p_float;
	float point;
	bool isNegative;
	virtAddr = machine->ReadRegister(4);
    buffer = new char[MAXLENGTH + 1];
    numbytes = gSynchConsole->Read(buffer, MAXLENGTH);
    //printf("numbytes: %d\n", numbytes);
    p_float = new float();
    *p_float = 0;
    i = 0;
    isNegative = false;
	if (buffer[0] == '-'){
		isNegative = true;
		i++;
	}
	for (;i < numbytes;i++){
		if (buffer[i] == '.'){
			i++;
			break;
		}
		else if (buffer[i] < '0' || buffer[i] > '9'){
			DEBUG('a', "Invalid floating point number.\n");
			*p_float = 0;
			i = numbytes;
			break;

		}
		else{
			*p_float = *p_float * 10 + (int)(buffer[i] - '0');
		}
	}
	point = 1;
	for (;i < numbytes;i++){
		point = point / 10;
		if (buffer[i] < '0' || buffer[i] > '9'){
			DEBUG('a', "Invalid floating point number.\n");
			*p_float = 0;
			break;

		}
		else{
			*p_float = *p_float + (int)(buffer[i] - '0') * point;
		}
	}
	if (isNegative){
		*p_float = -*p_float;
	}
	//printf("p_float: %f\n", *p_float);
	//printf("sizeof: %d\n", sizeof(point));
	System2User(virtAddr, 4, (char*)p_float);
	delete p_float;
	delete[] buffer;
}

void SC_PrintFloat_Handler(){
	int virtAddr, numbytes;
	char* buffer;
	char* str;
	float* p_float;
	float number;
	virtAddr = machine->ReadRegister(4);
	buffer = new char[sizeof(float) + 1];
	buffer = User2System(virtAddr, sizeof(float));
	p_float = (float*)buffer;
	number = *p_float;
	//printf("p_float: %f\n", *p_float);
    numbytes = snprintf(NULL, 0, "%f", number);
    p_float = (float*)buffer;
    number = *p_float;
	str = new char[numbytes + 1];
	snprintf(str, numbytes + 1, "%f", number); 
	//printf("numbytes: %d", numbytes);
	gSynchConsole->Write(str, numbytes + 1);
	delete[] str;
	//delete[] buffer;
	delete p_float;
}

void SC_ReadChar_Handler(){
	char* buffer;
	char ch;
	buffer = new char[2];
	gSynchConsole->Read(buffer, 2);//doc ky tu tren console
	ch = buffer[0];	
	machine->WriteRegister(2, ch);//ghi ky tu tra ve vao thanh ghi so 2	
	delete[] buffer;
}

void SC_PrintChar_Handler(){
	char ch;
	ch = machine->ReadRegister(4);//doc tham so cua ham tu thanh ghi so 4
	gSynchConsole->Write(&ch, 1);//ghi len console
}

void SC_ReadString_Handler(){
	int virtAddr, length, numbytes;
	char* buffer;
	virtAddr = machine->ReadRegister(4);//doc dia chi tham so buffer
    length = machine->ReadRegister(5);//doc length
    buffer = new char[MAXLENGTH + 1];
    numbytes = gSynchConsole->Read(buffer, MAXLENGTH);//doc chuoi ky tu tren console
    System2User(virtAddr, length, buffer);//chuyen du lieu tu kernelspace sang userspace
    delete[] buffer;
}

void SC_PrintString_Handler(){
	int virtAddr, length;
	char* buffer;
	virtAddr = machine->ReadRegister(4);//doc dia chi tham so buffer
	buffer = User2System(virtAddr, MAXLENGTH);//chuyen du lieu tu userspace sang kernelspace 
	length = strlen(buffer);//tinh do dai buffer	
	gSynchConsole->Write(buffer, length + 1);//ghi chuoi len console
	delete[] buffer;
}

void SC_CreateFile_Handler(){
	int virtAddr;
	char* filename;
	virtAddr = machine->ReadRegister(4);
	filename = User2System(virtAddr,MAXLENGTH+1);
	if (filename == NULL)
	{
		printf("\n Not enough memory in system");
		machine->WriteRegister(2,-1); // trả về lỗi cho chương trình người dùng
		delete[] filename;
		return;
	}
	// Create file with size = 0
	// Dùng đối tượng fileSystem của lớp OpenFile để tạo file, việc tạo file này là sử dụng các
	// thủ tục tạo file của hệ điều hành Linux, chúng ta không quản ly trực tiếp các block trên
	// đĩa cứng cấp phát cho file, việc quản ly các block của file trên ổ đĩa là một đồ án khác
	if (!fileSystem->Create(filename,0))
	{
		printf("\n Error create file '%s'",filename);
		machine->WriteRegister(2,-1);
		delete[] filename;
		return;
	}
	machine->WriteRegister(2,0); // trả về cho chương trình người dùng thành công
	delete[] filename;
}

void SC_Open_Handler(){
	int virtAddr, type, freeSlot;
	char* filename;
	virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
	type = machine->ReadRegister(5); // Lay tham so type tu thanh ghi so 5
	filename = User2System(virtAddr, MAXLENGTH); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
	//Kiem tra xem OS con mo dc file khong
	freeSlot = fileSystem->FindFreeSlot();
	if (freeSlot != -1) //Chi xu li khi con slot trong
	{
		if (type == 0 || type == 1) //chi xu li khi type = 0 hoac 1
		{
			if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) //Mo file thanh cong
			{
				machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
			}
		}
		else if (type == 2) // xu li stdin voi type quy uoc la 2
		{
			machine->WriteRegister(2, 0); //tra ve OpenFileID
		}
		else // xu li stdout voi type quy uoc la 3
		{
			machine->WriteRegister(2, 1); //tra ve OpenFileID
		}
		delete[] filename;
		return;
	}
	machine->WriteRegister(2, -1); //Khong mo duoc file return -1
	delete[] filename;
}

void SC_Read_Handler(){
	int virtAddr, charcount, fileID, prevPos, newPos, size;
	char* buffer;
	virtAddr = machine->ReadRegister(4);	// Lay dia chi cua buffer tu thanh ghi so 4
	charcount = machine->ReadRegister(5);	// Lay so ky tu can doc tu thanh ghi so 5
	fileID = machine->ReadRegister(6);		// Lay id cua file tu thanh ghi so 6

	// ID khong hop le  -> tra ve -1
	if (fileID < 0 || fileID > 14) {
		printf("\nFile ID khong hop le, khong the doc file!");
		machine->WriteRegister(2, -1);
		return;
	}
	// File khong ton tai -> tra ve -1
	if (fileSystem->openf[fileID] == NULL) {
		printf("\nFile khong ton tai, khong the doc file!");
		machine->WriteRegister(2, -1);
		return;
	}
	// File STDOUT -> tra ve -1
	if (fileSystem->openf[fileID]->type == STDOUT) {
		printf("\nKhong the doc file stdout!");
		machine->WriteRegister(2, -1);
		return;
	}

	// Kiem tra thanh cong
	prevPos = fileSystem->openf[fileID]->GetCurrentPos(); 
	buffer = User2System(virtAddr, charcount); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
	
	// File STDIN
	if (fileSystem->openf[fileID]->type == STDIN) {
		size = gSynchConsole->Read(buffer, charcount);	// Goi ham Read cua SynchConsole de doc chuoi tu ban phim
		System2User(virtAddr, size, buffer);				// Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la size
		machine->WriteRegister(2, size);					// Tra ve so byte thuc su doc duoc
		delete[] buffer;
		return;
	}
	// File binh thuong
	if (fileSystem->openf[fileID]->type == READ_WRITE 
		|| fileSystem->openf[fileID]->type == READ_ONLY) {
		// File co noi dung de doc -> tra ve so byte doc duoc
		if ((fileSystem->openf[fileID]->Read(buffer, charcount)) > 0) {
			newPos = fileSystem->openf[fileID]->GetCurrentPos();	// So byte thuc su doc duoc = newPos - prevPos
			System2User(virtAddr, newPos - prevPos, buffer);			// Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
			machine->WriteRegister(2, newPos - prevPos);				// Tra ve so byte thuc su doc duoc
		}
		// File khong co noi dung de doc -> tra ve -2
		else {
			machine->WriteRegister(2, -2); 
		}
	}
	delete[] buffer;
}

void SC_Write_Handler(){
	int virtAddr, charcount, fileID, prevPos, newPos, i;
	char* buffer;
	virtAddr = machine->ReadRegister(4);	// Lay dia chi cua buffer tu thanh ghi so 4
	charcount = machine->ReadRegister(5);	// Lay so ky tu can ghi tu thanh ghi so 5
	fileID = machine->ReadRegister(6);		// Lay id cua file tu thanh ghi so 6

	// ID khong hop le -> tra ve -1
	if (fileID < 0 || fileID > 14) {
		printf("\nFile ID khong hop le, khong the doc file!");
		machine->WriteRegister(2, -1);
		return;
	}
	// File khong ton tai -> tra ve -1
	if (fileSystem->openf[fileID] == NULL) {
		printf("\nFile khong ton tai, khong the doc file!");
		machine->WriteRegister(2, -1);
		return;
	}

	// File STDIN hoac file chi doc -> tra ve -1
	if (fileSystem->openf[fileID]->type == READ_ONLY || fileSystem->openf[fileID]->type == STDIN)
	{
		printf("\nKhong the write file stdin hoac file chi doc!");
		machine->WriteRegister(2, -1);
		return;
	}
	
	// Kiem tra thanh cong
	prevPos = fileSystem->openf[fileID]->GetCurrentPos(); 
	buffer = User2System(virtAddr, charcount);  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
	// File Read Write -> tra ve so byte thuc su ghi duoc
	if (fileSystem->openf[fileID]->type == READ_WRITE) {
		if ((fileSystem->openf[fileID]->Write(buffer, charcount)) > 0) {
			newPos = fileSystem->openf[fileID]->GetCurrentPos();	// So byte thuc su ghi duoc = newPos - prevPos
			machine->WriteRegister(2, newPos - prevPos);			// Tra ve so byte thuc su ghi duoc
			delete[] buffer;
			return;
		}
	}
	// File STDOUT
	if (fileSystem->openf[fileID]->type == STDOUT) {
		i = 0;
		while (buffer[i] != 0 && buffer[i] != '\n') {
			gSynchConsole->Write(buffer + i, 1); // Su dung ham Write cua lop SynchConsole 
			i++;
		}
		buffer[i] = '\n';
		gSynchConsole->Write(buffer + i, 1);	// Write ky tu '\n'
		machine->WriteRegister(2, i - 1);		// Tra ve so byte thuc su write duoc
	}
	delete[] buffer;
}

void SC_Close_Handler(){
	int fileID;
	fileID = machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
	if (fileID >= 0 && fileID <= 14) //Chi xu li khi fid nam trong [0, 14]
	{
		if (fileSystem->openf[fileID]) //neu mo file thanh cong
		{
			delete fileSystem->openf[fileID]; //Xoa vung nho luu tru file
			fileSystem->openf[fileID] = NULL; //Gan vung nho NULL
			machine->WriteRegister(2, 0);
			return;
		}
	}
	machine->WriteRegister(2, -1);
}

void SC_Seek_Handler(){
	int pos, fileID;
	pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
	fileID = machine->ReadRegister(5); // Lay id cua file
	// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
	if (fileID < 0 || fileID > 14)
	{
		printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
		machine->WriteRegister(2, -1);
		return;
	}
	// Kiem tra file co ton tai khong
	if (fileSystem->openf[fileID] == NULL)
	{
		printf("\nKhong the seek vi file nay khong ton tai.");
		machine->WriteRegister(2, -1);
		return;
	}
	// Kiem tra co goi Seek tren console khong
	if (fileID == 0 || fileID == 1)
	{
		//printf("\nKhong the seek tren file console.");
		machine->WriteRegister(2, -1);
		return;
	}
	// Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
	pos = (pos == -1) ? fileSystem->openf[fileID]->Length() : pos;
	if (pos > fileSystem->openf[fileID]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
	{
		printf("\nKhong the seek file den vi tri nay.");
		machine->WriteRegister(2, -1);
	}
	else
	{
		// Neu hop le thi tra ve vi tri di chuyen thuc su trong file
		fileSystem->openf[fileID]->Seek(pos);
		machine->WriteRegister(2, pos);
	}
}

void SC_FloatToString_Handler(){
	int virtAddr, virtAddr1, numbytes;
	char* buffer;
	char* str;
	float* p_float;
	float point;
	virtAddr = machine->ReadRegister(4);
	virtAddr1 = machine->ReadRegister(5);
	buffer = User2System(virtAddr1, 4);
	p_float = (float*)buffer;
	point = *p_float;
	//printf("float: %f\n", point);
	numbytes = snprintf(NULL, 0, "%f", point);
	//printf("numbytes: %d\n", numbytes);
    p_float = (float*)buffer;
    point = *p_float;
	str = new char[numbytes + 1];
	snprintf(str, numbytes + 1, "%f", point); 
	System2User(virtAddr, numbytes + 1, str);
	machine->WriteRegister(2, numbytes);
}

void SC_Exec_Handler(){
	int virtAddr, id;
	char* name;
	virtAddr = machine->ReadRegister(4);
	name = User2System(virtAddr, MAXLENGTH + 1); // Lay ten chuong trinh, nap vao kernel
	
	// Kiem tra ten chuong trinh rong
	if(name == NULL)
	{
		DEBUG('a', "\nCan not execute program with null name");
		printf("\nCan not execute program with null name");
		machine->WriteRegister(2, -1);
		return;
	}
	
	// Mo file
	OpenFile *oFile = fileSystem->Open(name);
	//Nếu bị lỗi thì báo “Không mở được file” và gán -1 vào thanh ghi 2.
	if (oFile == NULL)
	{
		DEBUG('a', "\nSC_Exec: Can't open this file.");
		printf("\nSC_Exec: Can't open this file.");
		machine->WriteRegister(2,-1);
		return;
	}

	delete oFile;

	// Return child process id
	id = pTab->ExecUpdate(name); 
	machine->WriteRegister(2, id);

	delete[] name;
}

void SC_Join_Handler(){
	int id, res;
	id = machine->ReadRegister(4); // Đọc id của tiến trình cần Join từ thanh ghi r4.
	// Gọi thực hiện pTab->JoinUpdate(id) và lưu kết quả thực hiện của hàm vào thanh ghi r2.
	res = pTab->JoinUpdate(id);

	machine->WriteRegister(2, res);
}

void SC_Exit_Handler(){
	int exitStatus, res;
	exitStatus = machine->ReadRegister(4); // Đọc exitStatus từ thanh ghi r4

	if(exitStatus != 0)
		return;		
	// Gọi thực hiện pTab->ExitUpdate(exitStatus)
	res = pTab->ExitUpdate(exitStatus);

	currentThread->FreeSpace();
	currentThread->Finish();
	
	machine->WriteRegister(2, res);				
}

void SC_CreateSemaphore_Handler(){
	int virtAddr, semval, res;
	char* name;
	virtAddr = machine->ReadRegister(4); //Đọc địa chỉ “name” từ thanh ghi r4.
	semval = machine->ReadRegister(5); //Đọc giá trị “semval” từ thanh ghi r5.
											
	//"name" lúc này đang ở trong user space. Gọi hàm User2System đã được khai báo 
	//để chuyển vùng nhớ user space tới vùng nhớ system space.
	name = User2System(virtAddr, MAXLENGTH + 1); // Lay ten Semaphore, nap vao kernel
	
	// Kiem tra name rong
	if(name == NULL)
	{
		DEBUG('a', "\nCan not create semaphore with null name");
		printf("\nCan not create semaphore with null name");
		machine->WriteRegister(2, -1);
		return;
	}					
	
	//Gọi thực hiện hàm semTab->Create(name,semval) để tạo Semaphore, nếu có lỗi thì báo lỗi.
	res = semTab->Create(name, semval);

	if(res == -1){
		DEBUG('a', "\nCan not create semaphore (%s, %d)", name, &semval);
		printf("\nCan not create semaphore (%s, %d)", name, &semval);
		machine->WriteRegister(2, -1);
		delete[] name;
		return;	
	}
	//Lưu kết quả thực hiện vào thanh ghi r2
	machine->WriteRegister(2, res);
	delete[] name;
}

void SC_Down_Handler(){
	int virtAddr, res;
	char* name;
	virtAddr = machine->ReadRegister(4); //Đọc địa chỉ “name” từ thanh ghi r4.
	//Tên địa chỉ “name” lúc này đang ở trong user space. Gọi hàm User2System 
	//để chuyển vùng nhớ user space tới vùng nhớ system space.
	name = User2System(virtAddr, MAXLENGTH + 1); // Lay ten Semaphore, nap vao kernel
	
	// Kiem tra name rong
	if(name == NULL)
	{
		DEBUG('a', "\nCan not create semaphore with null name");
		printf("\nCan not create semaphore with null name");
		machine->WriteRegister(2, -1);
		return;
	}

	//Kiểm tra Semaphore “name” này có trong bảng sTab chưa, nếu chưa có thì báo lỗi.
	//Gọi phương thức Wait() của lớp Stable.
	res = semTab->Wait(name);

	if(res == -1){
		DEBUG('a', "\nCan not down semaphore (%s)", name);
		printf("\nCan not down semaphore (%s)", name);
		machine->WriteRegister(2, -1);
		delete[] name;
		return;	
	}
	//Lưu kết quả thực hiện vào thanh ghi r2.
	machine->WriteRegister(2, res);
	delete[] name;
}

void SC_Up_Handler(){
	int virtAddr, res;
	char* name;
	virtAddr = machine->ReadRegister(4); //Đọc địa chỉ “name” từ thanh ghi r4.
	//Tên địa chỉ “name” lúc này đang ở trong user space. Gọi hàm User2System 
	//để chuyển vùng nhớ user space tới vùng nhớ system space.
	
	name = User2System(virtAddr, MAXLENGTH + 1); // Lay ten Semaphore, nap vao kernel
	
	// Kiem tra name rong
	if(name == NULL)
	{
		DEBUG('a', "\nCan not create semaphore with null name");
		printf("\nCan not create semaphore with null name");
		machine->WriteRegister(2, -1);
		return;
	}	
	
	//Kiểm tra Semaphore “name” này có trong bảng sTab chưa, nếu chưa có thì báo lỗi.
	//Gọi phương thức Signal() của lớp Stable.
	res = semTab->Signal(name);

	if(res == -1){
		DEBUG('a', "\nCan not up semaphore (%s)", name);
		printf("\nCan not up semaphore (%s)", name);
		machine->WriteRegister(2, -1);
		delete[] name;
		return;	
	}
	//Lưu kết quả thực hiện vào thanh ghi r2.
	machine->WriteRegister(2, res);
	delete[] name;
	return;
}

void
ExceptionHandler(ExceptionType which)
{

    int type = machine->ReadRegister(2);
    switch(which){
    	case NoException:
    		return;
    	case PageFaultException:
    		DEBUG('a', "PageFault dang xay ra.\n");
    		interrupt->Halt();
    	case ReadOnlyException:
    		DEBUG('a', "ReadOnly dang xay ra.\n");
    		interrupt->Halt();
    	case BusErrorException:
    		DEBUG('a', "BusError dang xay ra.\n");
    		interrupt->Halt();
    	case AddressErrorException:
    		DEBUG('a', "AddressError dang xay ra.\n");
    		interrupt->Halt();
    	case OverflowException:
    		DEBUG('a', "Overflow dang xay ra.\n");
    		interrupt->Halt();
    	case IllegalInstrException:
    		DEBUG('a', "IllegalInstr dang xay ra.\n");
    		interrupt->Halt();
    	case NumExceptionTypes:
    		DEBUG('a', "NumberExceptionTypes dang xay ra.\n");
    		interrupt->Halt();
    	case SyscallException:
    		switch(type){
    			case SC_Halt:
    				DEBUG('a', "Shutdown, initiated by user program.\n");
   					interrupt->Halt();
   				case SC_Sub:
					SC_Sub_Handler();
					IncrementPC();
					return;
				case SC_ReadInt:
					SC_ReadInt_Handler();
					IncrementPC();//tang program counter
					return;
				case SC_PrintInt:
					SC_PrintInt_Handler();
                    IncrementPC();
                    return;        			
				case SC_ReadFloat:
					SC_ReadFloat_Handler();
					IncrementPC();
					return;
				case SC_PrintFloat:
					SC_PrintFloat_Handler();
					IncrementPC();
					return;
				case SC_ReadChar:
					SC_ReadChar_Handler();
					IncrementPC();
					return;
				case SC_PrintChar:
					SC_PrintChar_Handler();
					IncrementPC();
					return;
				case SC_ReadString:
				    SC_ReadString_Handler();
				    IncrementPC();
				    return;
				case SC_PrintString:
					SC_PrintString_Handler();
					IncrementPC();
					return;
				case SC_CreateFile:
					SC_CreateFile_Handler();
					IncrementPC();
					return;
				case SC_Open:
					SC_Open_Handler();
					IncrementPC();
					return;
				case SC_Read:
					SC_Read_Handler();
					IncrementPC();
					return;
				case SC_Write:
					SC_Write_Handler();
					IncrementPC();
					return;
				case SC_Close:
					SC_Close_Handler();
					//IncrementPC();
					return;
				case SC_Seek:
					SC_Seek_Handler();
					IncrementPC();
					return;
				case SC_FloatToString:
					SC_FloatToString_Handler();
					IncrementPC();
    				return;
    			case SC_Exec:
    				SC_Exec_Handler();
    				IncrementPC();
    				return;
    			case SC_Join:
    				SC_Join_Handler();
    				IncrementPC();
    				return;
    			case SC_Exit:
    				SC_Exit_Handler();
    				IncrementPC();
    				return;
    			case SC_CreateSemaphore:
    				SC_CreateSemaphore_Handler();
    				IncrementPC();
    				return;
    			case SC_Down:
    				SC_Down_Handler();
    				IncrementPC();
    				return;
    			case SC_Up:
    				SC_Up_Handler();
    				IncrementPC();
    				return;

			}
    }
}
