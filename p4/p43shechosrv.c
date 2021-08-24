#include "unp.h"

void sig_chld(int signo) {
	pid_t pid;
	int stat;

	while((pid=waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);

	return;
}

int main(int argc, char **argv) {
	int			listenfd, connfd;
	pid_t			cpid;
	socklen_t		clen;
	ssize_t 		n;
	char 			buf[MAXLINE];		
	struct sockaddr_in	caddr, saddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = 	AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = 	htons(SERV_PORT);

	Bind(listenfd, (SA *) &saddr, sizeof(saddr));

	Listen(listenfd, LISTENQ);

	Signal(SIGCHLD, sig_chld);

	for( ; ; ) {
		clen = sizeof(caddr);
		if((connfd = Accept(listenfd, (SA *) &caddr, &clen)) < 0) {
			if(errno == EINTR) continue;
			else err_sys("accept error");
		}

		if((cpid=Fork()) == 0) {
			Close(listenfd);
			
			/* str_echo start */
			redo:
				while((n=read(connfd, buf, MAXLINE)) > 0)
					Writen(connfd, buf, n);

				if(n<0 && errno == EINTR) goto redo;
				else if (n<0) err_sys("str_echo: read error");
			/* str_echo end */
			exit(0);
		}
		Close(connfd);
	}
}
