# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as

GCCDIR = ../../../gnu-decstation-ultrix/decstation-ultrix/2.95.3/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc -B../../../gnu-decstation-ultrix/
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt shell matmult sort sub int float char string createfile file help ascii quicksort mergesort ping pong pingpong

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.c > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

sub.o: sub.c
	$(CC) $(CFLAGS) -c sub.c
sub: sub.o start.o
	$(LD) $(LDFLAGS) start.o sub.o -o sub.coff
	../bin/coff2noff sub.coff sub

int.o: int.c
	$(CC) $(CFLAGS) -c int.c
int: int.o start.o
	$(LD) $(LDFLAGS) start.o int.o -o int.coff
	../bin/coff2noff int.coff int

float.o: float.c
	$(CC) $(CFLAGS) -c float.c
float: float.o start.o
	$(LD) $(LDFLAGS) start.o float.o -o float.coff
	../bin/coff2noff float.coff float

char.o: char.c
	$(CC) $(CFLAGS) -c char.c
char: char.o start.o
	$(LD) $(LDFLAGS) start.o char.o -o char.coff
	../bin/coff2noff char.coff char

string.o: string.c
	$(CC) $(CFLAGS) -c string.c
string: string.o start.o
	$(LD) $(LDFLAGS) start.o string.o -o string.coff
	../bin/coff2noff string.coff string

createfile.o: createfile.c
	$(CC) $(CFLAGS) -c createfile.c
createfile: createfile.o start.o
	$(LD) $(LDFLAGS) start.o createfile.o -o createfile.coff
	../bin/coff2noff createfile.coff createfile

file.o: file.c
	$(CC) $(CFLAGS) -c file.c
file: file.o start.o
	$(LD) $(LDFLAGS) start.o file.o -o file.coff
	../bin/coff2noff file.coff file

help.o: help.c
	$(CC) $(CFLAGS) -c help.c
help: help.o start.o
	$(LD) $(LDFLAGS) start.o help.o -o help.coff
	../bin/coff2noff help.coff help

ascii.o: ascii.c
	$(CC) $(CFLAGS) -c ascii.c
ascii: ascii.o start.o
	$(LD) $(LDFLAGS) start.o ascii.o -o ascii.coff
	../bin/coff2noff ascii.coff ascii

quicksort.o: quicksort.c
	$(CC) $(CFLAGS) -c quicksort.c
quicksort: quicksort.o start.o
	$(LD) $(LDFLAGS) start.o quicksort.o -o quicksort.coff
	../bin/coff2noff quicksort.coff quicksort

mergesort.o: mergesort.c
	$(CC) $(CFLAGS) -c mergesort.c
mergesort: mergesort.o start.o
	$(LD) $(LDFLAGS) start.o mergesort.o -o mergesort.coff
	../bin/coff2noff mergesort.coff mergesort

ping.o: ping.c
	$(CC) $(CFLAGS) -c ping.c
ping: ping.o start.o
	$(LD) $(LDFLAGS) start.o ping.o -o ping.coff
	../bin/coff2noff ping.coff ping

pong.o: pong.c
	$(CC) $(CFLAGS) -c pong.c
pong: pong.o start.o
	$(LD) $(LDFLAGS) start.o pong.o -o pong.coff
	../bin/coff2noff pong.coff pong

pingpong.o: pingpong.c
	$(CC) $(CFLAGS) -c pingpong.c
pingpong: pingpong.o start.o
	$(LD) $(LDFLAGS) start.o pingpong.o -o pingpong.coff
	../bin/coff2noff pingpong.coff pingpong
