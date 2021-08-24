#include "unp.h"

int main(int argc, char **argv) {
	int i, listenfd, connfd, sockfd, maxfd, maxi; 
	int cli[FD_SETSIZE], nready;
	fd_set rset, allset;
	struct sockaddr_in saddr, caddr;
	socklen_t clen;
	ssize_t n;
	char buf[MAXLINE];

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *) &saddr, sizeof(saddr));
	Listen(listenfd, LISTENQ);

	/* initializee */
	maxfd = listenfd;
	maxi = -1;
	for(i=0; i<FD_SETSIZE; i++) cli[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for( ; ; ) {
		rset = allset;
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);
	
		/* New Connection */
		if(FD_ISSET(listenfd, &rset)) {
			clen = sizeof(caddr);
			connfd = Accept(listenfd, (SA *) &caddr, &clen);

			for(i = 0; i < FD_SETSIZE; i++) {
				if(cli[i]<0) {
					cli[i] = connfd;
					break;
				}
			}
			if(i == FD_SETSIZE) err_quit("Maximum Client");
			FD_SET(connfd, &allset);
			if(connfd > maxfd) maxfd = connfd;
			if(i > maxi) maxi = i;

			if(--nready <= 0) continue;
		}

		/* Check */
		for(i = 0; i <= maxi; i++) {
			if((sockfd = cli[i]) < 0) continue;
			if(FD_ISSET(sockfd, &rset)) {
				if((n = Read(sockfd, buf, MAXLINE)) == 0) {
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					cli[i] = -1;
				} else {
					Writen(sockfd, buf, n);
				}

				if(--nready <= 0) break;
			}
		}
	}		
}	

