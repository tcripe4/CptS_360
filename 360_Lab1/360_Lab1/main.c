//
//  main.c
//  360_Lab1
//
//  Copyright © 2020 Travis Cripe. All rights reserved.
//
#include "Partition.c"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void myprintf(char *fmt, ...);
int rpu(u32 x, int BASE);
void prints(char *s);
int printu(u32 x);
int printd(int x);
int printx(u32 x);
int printo(u32 x);

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
//int *ip;
char *ctable = "0123456789ABCDEF";
//u16 *ip;

int main(int argc, char *argv[ ], char *env[ ])
{
    int fd;
    char buf[512];
    //123
    fd = open("vdisk", O_RDONLY);  // check fd value: -1 means open() failed
    read(fd, buf, 512); // read sector 0 into buf[ ]
    
    struct partition *p = (struct partition*)&buf[0x1BE];
    
    // Makes the struct a pointer and then gets the address of the partition to put it into the struct
    int check = 0;
   for (int i = 1; i < 8; i++)
    {
        myprintf("Partition : %d\n", i);
        myprintf(" Sys_Type = %x\n Start_Sector = %d\n NR_Sectors = %d\n",p->sys_type, p->start_sector, p->nr_sectors);

        
//
        if (p->sys_type == 5)
        {
            u32 temp = p->start_sector; // For use of keeping beginning of extended sector
            u32 p4 = temp; // Advances to further sectors

            lseek(fd, (long)(temp)*512, SEEK_SET);  // lseek to byte 123*512 OR sector 123
            read(fd, buf, 512);  // read sector 123 into buf[ ]
            check = 1; // Checks if end of linked list
            while (check == 1)
            {
                i++;
                
                p = (struct partition*)&buf[0x1BE]; // Updates current struct to new partition info

                myprintf("Partition : %d\n", i);
                myprintf(" Sys_Type = %x\n Start_Sector = %d\n NR_Sectors = %d\n", p->sys_type, p4 + p->start_sector, p->nr_sectors);

                p++; // Advances to next partition

                p4 = temp + p->start_sector; // Updates the start sector of current partition

                if (p->start_sector == 0)
                {
                    myprintf("Exit\n");
                    check = 0;
                }

                lseek(fd, (long)(p4)*512, SEEK_SET); // New partition table
                read(fd, buf, 512);
            }
        }
        p++; // Advances to next Partition
    }
    myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n",
             'A', "this is a test", 100,    100,   100,  -100);
}

int rpu(u32 x, int BASE)
{
    char c;
    if (x){
        c = ctable[x % BASE];
        rpu(x / BASE, BASE);
        putchar(c);
    }
}

void prints(char *s)
{
    while (*s)
    {
        putchar(*s);
        s++;
    }
    //rpu(*s, 2);
}

int printu(u32 x)
{
    (x==0)? putchar('0') : rpu(x, 10);
    putchar(' ');
}

int printd(int x)
{
    if (x < 0)
    {
        putchar('-');
        x /= -1;
        printu(x);
    }
    else{
        printu(x);
    }
}

int printx(u32 x)
{
    putchar('0');
    putchar('x');
    rpu(x, 16);
    putchar(' ');
}

int printo(u32 x)
{
    putchar('0');
    rpu(x, 8);
    putchar(' ');
}

void myprintf(char *fmt, ...)
{
    char *cp = fmt;
    
    int *ip = (int *)&fmt + 1;
    
    while (*cp)
    {
        if (*cp != '%')
        {
            putchar(*cp);
            if (*cp == '\n')
            {
                putchar('\r');
            }
        }
        else
        {
            cp++;
            switch(*cp)
            {
                case 'c':
                    putchar(*ip);
                    ip++;
                    break;
                case 's':
                    prints((char*)*ip);
                    ip++;
                    break;
                case 'u':
                    printu(*ip);
                    ip++;
                    break;
                case 'd':
                    printd(*ip);
                    ip++;
                    break;
                case 'x':
                    printx(*ip);
                    ip++;
                    break;
                case 'o':
                    printo(* (u32 *)ip);
                    ip++;
                    break;
            }
        }
        cp++;
    }
}

