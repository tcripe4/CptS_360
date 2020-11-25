#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <fcntl.h>


#define MAX   256
#define PORT 1234

char line[MAX], ans[MAX];
int n;
char buf[MAX];

int cfd, sport;
char* sip;
struct sockaddr_in saddr;
struct hostent* hp;

int client_init(char* argv[])
{
	printf("1. get server info\n");
	hp = gethostbyname(argv[1]);
	sip = inet_ntoa(*((struct in_addr*)hp->h_addr));
	sport = PORT;

	printf("hostname= %s IP= %s port= %d\n", hp->h_name, sip, sport);

	printf("2. create a TCP socket\n");
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd < 0) {
		printf("socket creation failed\n");
		exit(1);
	}

	printf("3. fill in [localhost IP port=1234] as server address\n");
	//bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	//saddr.sin_addr.s_addr = sip;
	inet_pton(AF_INET, sip, &saddr.sin_addr);
	saddr.sin_port = htons(sport);


	printf("3. connect client socket to server\n");
	int r = connect(cfd, (struct sockaddr*) &saddr, sizeof(saddr));
	if (r < 0) {
		printf("connection to server failed\n");
		exit(2);
	}

	printf("hostname= %s IP= %s port= %d\n", hp->h_name, sip, sport);
}

int main(int argc, char *argv[ ]) 
{ 
	if (argc < 3) {
		printf("Usage : client ServerName SeverPort\n");
		exit(1);
	}

	client_init(argv);
    
    printf("********  processing loop  *********\n");
    while(1){
       printf("input a line : ");
       bzero(line, MAX);                // zero out line[ ]
       fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

       line[strlen(line)-1] = 0;        // kill \n at end
       if (line[0]==0)                  // exit if NULL line
          exit(0);

       // Send ENTIRE line to server
       n = write(cfd, line, MAX);
       printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

       // Read a line from sock and show it
       n = read(cfd, ans, MAX);
	   int size = atoi(ans);
       printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
     
       int count = 0;
       n = open(line, "w");
       while (count < size)
       {
           n = read(cfd, buf, MAX);
           count += n;
		   write(n, buf, MAX);
       }
    }
} 
