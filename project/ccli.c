#include "unp.h"

void str_cli(FILE *fp, int sockfd) {
	int 	n;
	int 	maxfdp1, stdineof;
	char 	buf[MAXLINE], sendbuf[MAXLINE];
	fd_set 	rset;

	stdineof = 0;
	FD_ZERO(&rset);
	for( ; ; ) {
		if(stdineof == 0) FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)) {
			if((n = Read(sockfd, buf, MAXLINE)) == 0) {
				if(stdineof == 1) return;
				else err_quit("SERVER Terminated\n");
			}
			Write(fileno(stdout), buf, n);
		}
		if(FD_ISSET(fileno(fp), &rset)) {
			if((n = Read(fileno(fp), buf, MAXLINE)) == 0) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			else if(n == 6 && (!strncmp(buf, "/exit", 5) || !strncmp(buf, "/quit", 5) ||
								!strncmp(buf, "/EXIT", 5) || !strncmp(buf, "/QUIT", 5))) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			Writen(sockfd, buf, n);
		}
	}
}

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in saddr;

	if(argc != 4)
			err_quit("usage: a.out <IPaddress> <PORT> <Nickname>");


	if((sockfd  = Socket(AF_INET, SOCK_STREAM, 0)) < 0)
			err_sys("Socket Error");

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(argv[2]));
	Inet_pton(AF_INET, argv[1], &saddr.sin_addr);

	Connect(sockfd, (SA *) &saddr, sizeof(saddr));
	Write(sockfd, argv[3], strlen(argv[3]));
	str_cli(stdin, sockfd);
	exit(0);
}
