#include "unp.h"

void dg_cli(FILE *fp, int sockfd, const SA *psaddr, socklen_t slen) {
	int n;
	char send[MAXLINE], recv[MAXLINE+1];

	while(Fgets(send, MAXLINE, fp) != NULL) {
		Sendto(sockfd, send, strlen(send), 0, psaddr, slen);

		n = Recvfrom(sockfd, recv, MAXLINE, 0, NULL, NULL);

		recv[n] = 0;
		Fputs(recv, stdout);
	}
}

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in saddr;

	if(argc != 2) err_quit("usage: udpcli <IPaddr>");

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &saddr.sin_addr);

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	dg_cli(stdin, sockfd, (SA *) &saddr, sizeof(saddr));

	exit(0);
}
