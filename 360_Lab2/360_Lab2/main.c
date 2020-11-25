#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>             // for I/O
#include <stdlib.h>            // for I/O
#include <libgen.h>            // for dirname()/basename()
#include <string.h>


typedef struct node {
	char  name[64];       // node's name string
	char  type;
	struct node* child, * sibling, * parent;
}NODE;


NODE* root, * cwd, * start;
char line[128];
char command[16], pathname[64];
char gpath[256];
char name[16][64];           // OR   char *name[16];
int n;


//               0       1      2        3      4        5       6
char* cmd[] = { "mkdir", "ls", "quit", "cd", "rmdir", "creat", "rm", 0 };

int findCmd(char* command)
{
	int i = 0;
	while (cmd[i]) {
		if (strcmp(command, cmd[i]) == 0)
			return i;
		i++;
	}
	return -1;
}

NODE* search_child(NODE* parent, char* name)
{
	NODE* p;
	printf("search for %s in parent DIR\n", name);
	p = parent->child;
	if (p == 0)
		return 0;
	while (p) {
		if (strcmp(p->name, name) == 0)
			return p;
		p = p->sibling;
	}
	return 0;
}

int rm(char* path)
{
	for (int i = 0; name[i][0] != '\0'; i++)
	{
		if (name[i + 1][0] == '\0')
		{
			NODE* p = search_child(cwd, name[i - 1]);
			if (p->child != 0 && p->child->name == 'F')
			{
				p->child = NULL;
			}
			else if (p->sibling != 0 && p->sibling->name == 'F');
			{
				p->sibling = NULL;
			}
		}
	}
	cwd = root;
}

int rmdir(char* path)
{
	for (int i = 0; pathname[i] != '\0'; i++)
	{
		if (pathname[i+1] == '\0')
		{
			char node_get[2];
			node_get[0] = pathname[i - 2];
			node_get[1] = '\0';
			NODE* p = search_child(cwd, node_get);
			if (p->child != 0 && p->child->name == 'D')
			{
				NODE* check = p->child;
				if (check->sibling != 0 || check->child != 0)
				{
					printf("Failed! Currently directory is not empty!\n");
					return;
				}
				p->child = NULL;
			}
			else if (p->sibling != 0 && p->sibling->name == 'D');
			{
				NODE* check = p->sibling;
				if (check->sibling != 0 || check->child != 0)
				{
					printf("Failed! Currently directory is not empty!\n");
					return;
				}
				p->sibling = NULL;
			}
		}
	}
	cwd = root;
}

int insert_child(NODE* parent, NODE* q)
{
	NODE* p;
	printf("insert NODE %s into parent child list\n", q->name);
	p = parent->child;
	if (p == 0)
		parent->child = q;
	else 
	{
		while (p->sibling)
			p = p->sibling;
		p->sibling = q;
	}
	q->parent = parent;
	q->child = 0;
	q->sibling = 0;
}

/***************************************************************
 This mkdir(char *name) makes a DIR in the current directory
 You MUST improve it to mkdir(char *pathname) for ANY pathname
****************************************************************/

int mkdir(char* name)
{
	NODE* p, * q;
	printf("mkdir: name=%s\n", name);

	if (!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, "..")) {
		printf("can't mkdir with %s\n", name);
		return -1;
	}
	if (name[0] == '/')
		start = root;
	else
		start = cwd;

	char check_node[2];
	int j = 0;
	while (name[j] != '\0')
	{
		j++;
	}
	check_node[0] = name[j - 1];
	check_node[1] = '\0';

	printf("check whether %s already exists\n", check_node);
	p = search_child(start, check_node);
	if (p) {
		printf("name %s already exists, mkdir FAILED\n", check_node);
		return -1;
	}
	printf("--------------------------------------\n");
	printf("ready to mkdir %s\n", check_node);
	int i = 0;

	q = (NODE*)malloc(sizeof(NODE));
	q->type = 'D';
	strcpy(q->name, check_node);
	insert_child(start, q);

	printf("mkdir %s OK\n", check_node);
	printf("--------------------------------------\n");

}

int creat(char* file_name)
{
	NODE* p, * q;
	p = cwd;
	printf("creat: name=%s\n", name);

	if (!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, "..")) {
		printf("can't creat with %s\n", name);
		return -1;
	}
	if (file_name[0] == '/')
		start = root;
	else
		start = cwd;

	printf("check whether %s already exists\n", name);

	char* name_file = "";

	for (int i = 0; name[i] != '\0'; i++)
	{
		if (name[1][0] == '\0')
		{
			if (start->sibling == 0 || start->child == 0)
			{
				q = (NODE*)malloc(sizeof(NODE));
				q->type = 'F';
				strcpy(q->name[0], name_file);
				insert_child(p, q);
			}
		}
		else if (name[i][0] == '\0')
		{
			p = search_child(start, name[i - 2]);
			name_file = name[i - 1];

			break;
		}
	}
	q = (NODE*)malloc(sizeof(NODE));
	q->type = 'F';
	strcpy(q->name, name_file);
	insert_child(p, q);
	//}
	
	printf("--------------------------------------\n");
	printf("ready to creat %s\n", name);
	int i = 0;

	
	//if (p->child == 0)
	//{
	//	p->child = q;
	//}
	//else
	//{
	//	p->sibling = q;
	//}

	printf("mkdir %s OK\n", name);
	printf("--------------------------------------\n");
}

// This ls() list CWD. You MUST improve it to ls(char *pathname)
int ls(char *pathname)
{
	NODE* p;
	int i = 0;
	while (name[i][0] != '\0')
	{
		i++;
	}
	printf("cwd contents = ");
	if (i == 0)
	{
		p = cwd;
		if (p->child != 0)
		{
			printf("[%c %s] ", p->child->type, p->child->name);
		}
		if (p->sibling != 0)
		{
			printf("[%c %s] ", p->sibling->type, p->sibling->name);
		}
	}
	else
	{
		p = search_child(cwd, name[i - 1]);
		if (p->child != 0)
		{
			printf("[%c %s] ", p->child->type, p->child->name);
		}
		if (p->sibling != 0)
		{
			printf("[%c %s] ", p->sibling->type, p->sibling->name);
		}
	}
	if (p->child == 0 && p->sibling == 0)
	{
		printf("Empty");
	}
	printf("\n");

}

int cd(char* path)
{
	NODE* p;
	int i = 0;
	while (path[i] != '\0')
	{
		i++;
	}
	char node_get[2];
	node_get[0] = path[i - 1];
	node_get[1] = '\0';
	p = search_child(start, node_get);

	if (strcmp(p->name, node_get) == 0)
	{
		printf("Found the directory!\n");
		cwd = p;
		*pathname = path;
	}
}

int quit()
{
	printf("Program exit\n");
	exit(0);
	// improve quit() to SAVE the current tree as a Linux file
	// for reload the file to reconstruct the original tree
}

int initialize()
{
	root = (NODE*)malloc(sizeof(NODE));
	strcpy(root->name, "/");
	root->parent = root;
	root->sibling = 0;
	root->child = 0;
	root->type = 'D';
	cwd = root;
	printf("Root initialized OK\n");
}

int print_commands()
{
	int i = 0;
	while (cmd[i])
	{
		printf(cmd[i]);
		i++;
		putchar(' ');
	}
	printf("\n");
}

int tokenize(char* pathname)
{
	int i;
	char* s;
	for (int i = 0; i < 15; i++)
	{
		*name[i] = NULL;
	}
	printf("pathname=%s\n", pathname);
	strcpy(name, "");
	strcpy(gpath, pathname);
	n = 0;
	s = strtok(gpath, "/");

	while (s) {
		strcpy(name[n++], s);    // OR  name[n++] = s;
		s = strtok(0, "/");
	}

	for (i = 0; i < n; i++) {
		printf("name[%d]=%s\n", i, name[i]);
	}
}

int main()
{
	int index;

	initialize();


	print_commands();

	//printf("NOTE: commands = [mkdir|ls|quit]\n");

	while (1) {
		printf("Enter command line : ");
		print_commands();
		fgets(line, 128, stdin);
		line[strlen(line) - 1] = 0;

		command[0] = pathname[0] = 0;
		sscanf(line, "%s %s", command, pathname);
		tokenize(pathname);
		printf("command=%s pathname=%s\n", command, pathname);

		if (command[0] == 0)
			continue;

		index = findCmd(command);

		switch (index) {
		case 0: mkdir(pathname); break;
		case 1: ls(pathname);            break;
		case 2: quit();          break;
		case 3: cd(pathname);	 break;
		case 4: rmdir(pathname); break;
		case 5: creat(pathname); break;
		case 6: rm(pathname);     break;
		}
	}
}







/********************************
NODE *search(NODE *parent, char *name)
{
  // same as before
}

NODE *path2node(char *pathname)
{
  NODE *start, *p;
  int i;

  1. tokenize(pathname);
  2. NODE *start = root OR cwd;
  3. for (i=0; i < n; i++){
		p = search(start, name[i]);
		if (p==0)
		   return 0;
		start = p;
	 }
  4. return p;
}
********************************/
