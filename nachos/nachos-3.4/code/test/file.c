
#include "syscall.h"
#define MAX_LENGTH 32
#define READ_WRITE 0
#define READ_ONLY 1
#define STDIN 2
#define STDOUT 3

int main()
{
    // OpenFileId cua file nguon va file dich
    int srcFileId;
    int destFileId;
    // Do dai file
    int fileSize;
    int i;  // Index for loop
    char c; // Ky tu de in ra
    char print[1024];
    int k = 0;
    srcFileId = Open("hello.txt", READ_WRITE); // Goi ham Open de mo file nguon

    if (srcFileId != -1) // Kiem tra mo file thanh cong
    {

        destFileId = Open("stdout", STDOUT); // Goi ham Open de mo file dich
        if (destFileId != -1)                  // Kiem tra mo file thanh cong
        {
            // Seek den cuoi file nguon de lay duoc do dai noi dung file nguon (fileSize)
            fileSize = Seek(-1, srcFileId);

            // Chuan bi sao chep
            Seek(0, srcFileId);  // Seek den dau file nguon
            Seek(0, destFileId); // Seek den dau file dich
            i = 0;

            // Vong lap chay tu dau file nguon den het file nguon
            for (; i < fileSize; i++)
            {
                Read(&c, 1, srcFileId);   // Doc tung ki tu cua file nguon
                print[k] = c;
                k++;
                if (c == '\n')
                {
                    print[k] = '\0';
                    Write(print, k, destFileId); // Ghi ki tu vua doc ra file dich
                    k = 0;
                }
            }   
            Close(destFileId); // Goi ham Close de dong file dich
        }
        else
            PrintString(" -> Tao file dich khong thanh cong!\n\n");

        Close(srcFileId); // Goi ham Close de dong file nguon
    }
    else
        PrintString("\nLoi khi mo file");

    return 0;
}