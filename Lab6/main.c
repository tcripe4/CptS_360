/****************************************************************************
*                   KCW testing ext2 file system                            *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>
#include <ext2fs/ext2_fs.h>

#include "type.h"

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

char   gpath[256]; // global for tokenized components
char   *name[64];  // assume at most 64 components in pathname
int    n;          // number of component strings

int    fd, dev;
int    nblocks, ninodes, bmap, imap, inode_start;
char   line[256], cmd[32], pathname[256];

#include "util.c"
#include "cd_ls_pwd.c"

int init()
{
  int i, j;
  MINODE *mip;
  PROC   *p;

  printf("init()\n");

  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->uid = i;
    p->cwd = 0;
    p->status = READY;
    for (j=0; j<NFD; j++)
    {
      p->fd[j] = 0;
    }
    proc[i].next = &proc[0];
  }
    proc[NPROC-1].next = &proc[0];
    running = &proc[0];
    *root = 0;
}

// load root INODE and set root pointer to it
int mount_root()
{  
  int i;
  MTABLE *mp;
  SUPER *sp;
  GD *gp;
  char buf[BLKSIZE]
    
    dev = open(rootdev, O_RDWR);
    if (dev < 0){
    printf("panic : can’t open root device\n"); exit(1);
    }
    /* get super block of rootdev */ get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    /* check magic number */
    if (sp->s_magic != SUPER_MAGIC){
    printf("super magic=%x : %s is not an EXT2 filesys\n", sp->s_magic, rootdev);
    exit(0); }
    // fill mount table mtable[0] with rootdev information mp = &mtable[0]; // use mtable[0]
    mp->dev = dev;
    // copy super block info into mtable[0]
    ninodes = mp->ninodes = sp->s_inodes_count; nblocks = mp->nblocks = sp->s_blocks_count; strcpy(mp->devName, rootdev); strcpy(mp->mntName, "/");
    
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    bmap = mp->bmap = gp->bg_blocks_bitmap;
    imap = mp->imap = gp->bg_inodes_bitmap;
    iblock = mp->iblock = gp->bg_inode_table;
    printf("bmap=%d imap=%d iblock=%d\n", bmap, imap iblock);
    

    // call iget(), which inc minode’s refCount
    root = iget(dev, 2); mp->mntDirPtr = root; root->mntPtr = mp;
    // set proc CWDs
    for (i=0; i<NPROC; i++)
    proc[i].cwd = iget(dev, 2); // each inc refCount by 1
    printf("mount : %s mounted on / \n", rootdev);
    return 0;
}

char *disk = "mydisk";
int main(int argc, char *argv[ ])
{
  int ino;
  char buf[BLKSIZE];
  if (argc > 1)
    disk = argv[1];

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0){
    printf("open %s failed\n", disk);  exit(1);
  }
  dev = fd;
  /********** read super block at 1024 ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system *****************/
  if (sp->s_magic != 0xEF53){
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }     
  printf("OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(dev, 2, buf); 
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  inode_start = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, inode_start);

  init();  
  mount_root();

  printf("root refCount = %d\n", root->refCount);
  
  printf("creating P0 as running process\n");
  running = &proc[0];
  running->status = READY;
  running->cwd = iget(dev, 2);
  
  printf("root refCount = %d\n", root->refCount);

  //printf("hit a key to continue : "); getchar();
  while(1){
    printf("input command : [ls|cd|pwd|quit] ");
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;
    if (line[0]==0)
      continue;
    pathname[0] = 0;
    cmd[0] = 0;
    
    sscanf(line, "%s %s", cmd, pathname);
    printf("cmd=%s pathname=%s\n", cmd, pathname);

    if (strcmp(cmd, "ls")==0)
       list_file();
    if (strcmp(cmd, "cd")==0)
       change_dir();
    if (strcmp(cmd, "pwd")==0)
       pwd(running->cwd);

    if (strcmp(cmd, "quit")==0)
       quit();
  }
}
 
int quit()
{
  int i;
  MINODE *mip;
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount > 0)
      iput(mip);
  }
  exit(0);
}
