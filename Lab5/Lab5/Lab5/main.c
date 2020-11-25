//
//  main.c
//  Lab5
//
//  Created by Travis Cripe on 10/23/19.
//  Copyright © 2019 Travis Cripe. All rights reserved.
//
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <stdio.h>
#define BLKSIZE 1024
#define u32 uint32_t
char buf[BLKSIZE];
char ibuf[BLKSIZE];
int dev = 0;

typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

INODE *ip;
GD *gp;
SUPER *sp;
DIR *dp;

char *name[64];
char gline[256];
int nname;

int tokenize(char *pathname)
{
  char *s;
  strcpy(gline, pathname);
  nname = 0;
  s = strtok(gline, "/");
  while(s){
    name[nname++] = s;
      s = strtok(0, "/"); }
    }

int search(INODE *ip, char *name)
{
    int i;
    char *cp, temp[256], sbuf[BLKSIZE];
    DIR *dp;
    for (i=0; i<12; i++){ // search DIR direct blocks only
        if (ip->i_block[i] == 0)
        {
            return 0;
        }
        get_block(dev, ip->i_block[i], sbuf);
        dp = (DIR *)sbuf;
        cp = sbuf;
        while (cp < sbuf + BLKSIZE){
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            printf("%8d%8d%8u %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
        if (strcmp(name, temp)==0){
            printf("found %s : inumber = %d\n", name, dp->inode);
           return dp->inode;
        }
        cp += dp->rec_len;
        dp = (DIR *)cp; }
    }
    return 0;
}

int get_block(int dev, int blk, char *buf){// get_block() reads a disk BLOCK into a char buf[BLKSIZE].
    lseek(dev, blk*BLKSIZE, SEEK_SET);
    return read(dev, buf, BLKSIZE);
}

int show_dir(INODE *ip)
    {
       char sbuf[BLKSIZE], temp[256];
       char *cp;
       int i;

       for (i=0; i < 12; i++){  // assume DIR at most 12 direct blocks
           if (ip->i_block[i] == 0)
              break;
           // YOU SHOULD print i_block[i] number here
           printf("%X", ip->i_block[i]);
           get_block(dev, ip->i_block[i], sbuf);

           dp = (DIR *)sbuf;
           cp = sbuf;
           
           while(cp < sbuf + BLKSIZE){
              strncpy(temp, dp->name, dp->name_len);
              temp[dp->name_len] = 0;
              printf("%4d %4d %4d %s\n",
                dp->inode, dp->rec_len, dp->name_len, temp);
              cp += dp->rec_len;
              dp = (DIR *)cp;
          }
          
       }
    }

int main(){
    int curblk = 0, bmap, imap, i_nodesStart, filetype;
    u32 *ubuf;
    dev = open("diskimage.dms", O_RDONLY);   // OR  O_RDWR
    get_block(dev, 1, buf);
    sp = (SUPER *) buf;
    ubuf = buf;
    filetype = ubuf[14];
    printf("\nfiletype = %X\n", filetype);
    get_block(dev, 2, buf);
    gp = (GD*) buf;
    printf("bmap = %d, imap = %d, i_nodes start = %d\n", gp->bg_block_bitmap, gp->bg_inode_bitmap, gp->bg_inode_table);
    get_block(dev, gp->bg_inode_table, ibuf);
    ip = (INODE *)ibuf + 1;
    int in = show_dir(ip);
    printf("Input a directory that you want to look for: \n");
    char direc[100];
    scanf("%s", direc);
    tokenize(direc);

    int ino, blk, offset;
    int inodes_start = gp->bg_inode_table;
    for (int i=0; i < nname; i++)
    {
        ino = search(ip, name[i]);
          
        if (ino==0)
        {
            printf("can't find %s\n", name[i]);
            exit(1);
        }
    
        // Mailman's algorithm: Convert (dev, ino) to INODE pointer
        blk = (ino - 1) / 8 + inodes_start;
        offset = (ino - 1) % 8;
        get_block(dev, blk, ibuf);
        ip = (INODE *)ibuf + offset;   // ip -> new INODE
    }
    printf("The direct block numbers are: \n");
    for (int i = 0; i < 11; i++)
    {
        printf("%d ", ip->i_block[i]);
    }
    get_block(dev, ip->i_block[12], buf);
    ubuf = (u32 *)buf;
    printf("\nThe indirect block is: \n");
    for (int i = 0; i < 256; i++)
    {
        if (*ubuf != 0)
        {
            printf("%u ", *ubuf);
            ubuf++;
        }
    }
    printf("\n");
    char doub_buf[BLKSIZE];
    get_block(dev, ip->i_block[13], buf);
    ubuf = (u32 *)buf;
    printf("\nThe Double indirect block is: \n");
    u32 *doub_ubuf;
    for (int i = 0; i < 256; i++)
    {
        if (*ubuf != 0)
        {
            get_block(dev, ubuf, doub_buf);
            doub_ubuf = (u32 *) doub_buf;
            for (int j = 0; j < 256; j++)
            {
                if (*doub_ubuf != 0)
                {
                    printf("%u ", *doub_ubuf);
                    doub_ubuf++;
                }
            }
        ubuf++;
        }
        else if (ubuf == 0)
        {
            break;
        }
        
    }
    printf("\n");
    
}
