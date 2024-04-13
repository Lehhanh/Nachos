#include "syscall.h"

void merge(float* arr[], int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;
	float* left[n1];
	float* right[n2];

	for (i = 0; i < n1; i++)
		left[i] = arr[l + i];
	for (j = 0; j < n2; j++)
		right[j] = arr[m + 1 + j];
	i = 0;
	j = 0;
	k = l;
	while (i < n1 && j < n2) {
		if (left[i] < right[j]) {
			arr[k] = left[i];
			i++;
		}
		else {
			arr[k] = right[j];
			j++;
		}
		k++;
	}
	while (i < n1) {
		arr[k] = left[i];
		i++;
		k++;
	}
	while (j < n2) {
		arr[k] = right[j];
		j++;
		k++;
	}
}
void mergeSort(float* arr[], int l, int r)
{
	if (l < r) {
		int m = l + (r - l) / 2;
		mergeSort(arr, l, m);
		mergeSort(arr, m + 1, r);
		merge(arr, l, m, r);
	}
}


int main() {
	float f[100];
	char buffer[200];
	char str[50];
	int n, i, j, k, fileID, size, length;

	PrintString("Nhap so phan tu: ");
	n = ReadInt();
	PrintString("Nhap chuoi so thuc:\n");
	for (i = 0; i < n; ++i) {
		ReadFloat(&f[i]);
	}

	mergeSort(f, 0, n - 1);

	fileID = Open("mergesort.txt", 0);

	k = 0;
	length = 0;
	for (i = 0; i < n; i++) {
		size = FloatToString(str, &f[i]);
		j = 0;
		for (;j < size;j++){
			buffer[k++] = str[j];
		}
		buffer[k++] = ' ';
		length += size + 1;
	}
	buffer[length] = '\0';
	Write(buffer, length, fileID);
	Close(fileID);
	Halt();
}