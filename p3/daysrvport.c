#include	"unp.h"
#include	<time.h>

int
main(int argc, char **argv)
{
	int			listenfd, connfd;
	struct sockaddr_in	servaddr, cliaddr;
	socklen_t		len;
	char			buff[MAXLINE];
	time_t			ticks;
	pid_t			pid;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	if (argc != 2)
		err_quit("usage: a.out <Port>");		

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));	/* daytime server */

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &len);
		printf("connection from %s, port %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

		if ((pid=fork())==0) {
			Close(listenfd);
        		
			ticks = time(NULL);
        		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        		Write(connfd, buff, strlen(buff));
			
			Close(connfd);
			exit(0);
		}

		Close(connfd);
	}
}
