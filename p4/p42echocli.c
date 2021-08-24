#include "unp.h"

int main(int argc, char **argv) {
	int 			sockfd;
	char			send[MAXLINE], recv[MAXLINE];
	struct sockaddr_in	saddr;

	if (argc != 2)
		err_quit("usage: ./p42echocli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &saddr.sin_addr);

	Connect(sockfd, (SA *) &saddr, sizeof(saddr));

	/* str_cli start */
	while(Fgets(send, MAXLINE, stdin) != NULL) {
		Writen(sockfd, send, strlen(send));

		if(Readline(sockfd, recv, MAXLINE) == 0)
			err_quit("server terminated prematurely");

		Fputs(recv, stdout);
	}
	exit(0);
}
