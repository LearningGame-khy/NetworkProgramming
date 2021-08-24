#include "unp.h"

void dg_echo(int sockfd, SA *pcaddr, socklen_t clen) {
	int n;
	socklen_t len;
	char buf[MAXLINE];

	for( ; ; ) {
		len = clen;
		n = Recvfrom(sockfd, buf, MAXLINE, 0, pcaddr, &len);

		Sendto(sockfd, buf, n, 0, pcaddr, len);
	}
}

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in saddr, caddr;

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(SERV_PORT);

	Bind(sockfd, (SA *) &saddr, sizeof(saddr));

	dg_echo(sockfd, (SA *) &caddr, sizeof(caddr));
}
