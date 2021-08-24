/* TCP echo client with select() */
#include "unp.h"

void str_cli(FILE *fp, int sockfd) {
	int maxfdp1;
	fd_set rset;
	char send[MAXLINE], recv[MAXLINE];

	FD_ZERO(&rset);
	for( ; ; ) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)) {
			if(Readline(sockfd, recv, MAXLINE) == 0)
				err_quit("str_cli: server terminated");
			Fputs(recv, stdout);
		}

		if(FD_ISSET(fileno(fp), &rset)) {
			if(Fgets(send, MAXLINE, fp) == NULL) return;
			Writen(sockfd, send, strlen(send));
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
