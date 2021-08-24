#include "unp.h"

void sig_chld(int signo) {
	pid_t pid;
	int stat;

	while((pid=waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("Child %d Terminated\n", pid);
	}

}


int main(int argc, char **argv) {
	int listenfd, connfd, udpfd, nready, maxfdp1;
	char buf[MAXLINE];
	pid_t cpid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in caddr, saddr;
	void sig_chld(int);

	/* TDP Socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(SERV_PORT);

	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	Bind(listenfd, (SA *) &saddr, sizeof(saddr));

	Listen(listenfd, LISTENQ);

	/* UDP Socket */
	udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(SERV_PORT);

	Bind(udpfd, (SA *) &saddr, sizeof(saddr));
	Signal(SIGCHLD, sig_chld);

	FD_ZERO(&rset);
	maxfdp1 = max(listenfd, udpfd)+1;
	for( ; ; ) {
		FD_SET(listenfd, &rset);
		FD_SET(udpfd,&rset);
		if((nready=select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
			if(errno == EINTR) continue;
			else err_sys("Select Error");
		}

		if(FD_ISSET(listenfd, &rset)) {
			len = sizeof(caddr);
			connfd = Accept(listenfd, (SA *) &caddr, &len);

			if((cpid = Fork()) == 0) {
				Close(listenfd);
				str_echo(connfd);
				exit(0);
			}
			Close(connfd);
		}

		if(FD_ISSET(udpfd, &rset)) {
			len = sizeof(caddr);
			n = Recvfrom(udpfd, buf, MAXLINE, 0, (SA *) &caddr, &len);

			Sendto(udpfd, buf, n, 0, (SA *) &caddr, len);
		}
	}
}
