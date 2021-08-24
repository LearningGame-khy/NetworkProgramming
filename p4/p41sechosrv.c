#include "unp.h"

int main(int argc, char **argv) {
	int			listenfd, connfd;
	pid_t			cpid;
	socklen_t		clen;
	struct sockaddr_in	caddr, saddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = 	AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = 	htons(SERV_PORT);

	Bind(listenfd, (SA *) &saddr, sizeof(saddr));

	Listen(listenfd, LISTENQ);

	for( ; ; ) {
		ssize_t n;
		char buf[MAXLINE];		

		clen = sizeof(caddr);
		connfd = Accept(listenfd, (SA *) &caddr, &clen);

		if((cpid=Fork()) == 0) {
			Close(listenfd);
			
			/* str_echo start */
			redo:
				while((n=read(connfd, buf, MAXLINE)) > 0)
					Writen(connfd, buf, n);

				if(n<0 && errno == EINTR)
					goto redo;
				else if (n<0)
					err_sys("str_echo: read error");
			/* str_echo end */
			exit(0);
		}
		Close(connfd);
	}
}
