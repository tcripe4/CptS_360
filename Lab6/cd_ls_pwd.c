/************* cd_ls_pwd.c file **************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern char   gpath[256];
extern char   *name[64];
extern int    n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256];
char *cwd;
#include <unistd.h>
#define OWNER  000700
#define GROUP  000070
#define OTHER  000007
char* t1 = "xwrxwrxwr-------";
char* t2 = "----------------";

int change_dir(char *pathname)
{
	int ino = getino(pathname);

	if (ino == 0)	//return error if ino = 0
		return 0;

	MINODE *mip = iget(dev, ino);

	if (!S_ISDIR(mip->INODE.i_mode))	//return error if mip is not DIR
	{
		printf("ERROR\n");
		iput(mip);
		return 0;
	}

	iput(running->cwd);	//release old cwd;
	running->cwd = mip; // change cwd to mip
}


int ls(char *pathname)
{
	int ino = getinto(pathname);
	MINODE *mip = iget(dev, ino);
	int r;
	char *path;
	pathname = "./";

	if (r = lstat(pathname, mip->ino->i_block[0]) < 0)
	{
		printf("no such file %s \n", pathname);
		exit(1);
	}

	strcpy(path, pathname);
	if (path[0] != '/')
	{
		getcwd(cwd, 256)
		strcpy(path, cwd);
		strcat(path, "/");
		strcat(path, pathname);

//		if (entry[1].value != NULL)
//		{
//			strcat(path, "/");
//			strcat(path, entry[1].value);
//		}
	}

	if (S_ISDIR(mip->ino->i_mode))
	{
		ls_dir(path);
	}

	else
	{
		ls_file(path);
	}

}

int ls_file(int ino, char *name)
{
	MINODE *mip = iget(dev, ino);
	int r, i;
	char ftime[64];

	if ((mip->INODE->i_mode & 0xF000) == 0x8000) // if (S_ISREG())
	{
		printf("%c", '-');
	}

	else if ((mip->INODE->i_mode & 0xF000) == 0x4000) // if (S_ISDIR())
	{
		printf("%c", 'd');
	}

	else if ((mip->INODE->i_mode & 0xF000) == 0xA000) // if (S_ISLNK())
	{
		printf("%c", 'l');
	}

	for (i = 8; i >= 0; i--)
	{
		if (mip->INODE->i_mode & (1 << i)) // print r|w|x
		{
			printf("%c", t1[i]);
		}

		else
		{
			printf("%c", t2[i]); // or print -
		}
	}
	printf("%4d ", mip->ino->i_uid);  // uid
	printf("%8d ", mip->ino->st_size); // size

	strcpy(ftime, ctime(&mip->ino->ctime)); // print time in calendar form
	ftime[strlen(ftime) - 1] = 0; // kill \n at end
	printf("%s ", ftime); // print name
	printf("%s", basename(fname)); // print file basename // print -> linkname if symbolic file

	if ((mip->ino->i_mode & 0xF000) == 0xA000)
	{
		char* linkname = "";
		r = readlink(fname, linkname, 16);
		printf(" -> %s", linkname);
	}
	printf("\n");
}

int ls_dir(char *dirname)
{
	int ino = getinto(dirname);
	MINODE *mip = iget(dev, ino);
	char sbuf[BLKSIZE], temp[256];
	char *cp;
	int i;

	for (i = 0; i < 12; i++) {  // assume DIR at most 12 direct blocks
		if (mip->INODE.i_block[i] == 0)
			break;
		// YOU SHOULD print i_block[i] number here
		get_block(dev, mip->INODE.i_block[i], sbuf);

		dp = (DIR *)sbuf;
		cp = sbuf;

		while (cp < sbuf + BLKSIZE) {
			strncpy(temp, dp->name, dp->name_len);
			temp[dp->name_len] = 0;
			printf("%4d %4d %4d %s\n",
				dp->inode, dp->rec_len, dp->name_len, temp);
			ls_file(dp, temp);
			cp += dp->rec_len;
			dp = (DIR *)cp;
		}
	}
}



int pwd(MINODE *wd)
{
	if (wd == root)
		printf("/\n");
	else
		rpwd(wd);
}

int rpwd(MINODE *wd)
{
	int my_ino, parent_ino;
	char myname[256], buf[BLKSIZE], *cp;
	MINODE *pip; 

	if (wd == root)
		return;

	get_block(dev, wd->INODE.i_block[0], buf);
	dp = (DIR *)buf;
	cp = buf;

	my_ino = dp->inode;

	cp += dp->rec_len;
	dp = (DIR *)cp;

	parent_ino = dp->inode;
	pip = iget(dev, parent_ino);

	// using show dir function to get the string name from dir as LOCAL
	pwd_dir(pip, myname, buf, my_ino);
	
	rpwd(pip);

	printf("/%s", myname);
}

int pwd_dir(MINODE* ip, char myname[], char sbuf[], int my_ino)
{
	char* cp;
	int i;

	for (i = 0; i < 12; i++) {  // assume DIR at most 12 direct blocks
		if (ip->INODE.i_block[i] != 0) {
			get_block(dev, ip->INODE.i_block[i], sbuf);

			dp = (DIR*)sbuf;
			cp = sbuf;

			while (cp < sbuf + BLKSIZE) {
				// finidng the current dir
				if (dp->inode == my_ino) {
					strncpy(myname, dp->name, dp->name_len);
					myname[dp->name_len] = 0;
				}
				cp += dp->rec_len;
				dp = (DIR*)cp;
			}
		}
	}
}


