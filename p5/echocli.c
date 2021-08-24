/* TCP echo client with select() */
#include "unp.h"

void str_cli(FILE *fp, int sockfd) {
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;

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
				else err_quit("str_cli: server terminated");
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
			Writen(sockfd, buf, n);
		}
	}
}

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in saddr;

	if(argc != 2)
		err_quit("usage: /a.out <IPaddr>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &saddr.sin_addr);

	Connect(sockfd, (SA *) &saddr, sizeof(saddr));
	str_cli(stdin, sockfd);
	exit(0);
}
