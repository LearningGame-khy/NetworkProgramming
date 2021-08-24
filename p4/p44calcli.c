#include "unp.h"

struct args {
	long arg1;
	long arg2;
};

struct result {
	long sum;
};

void str_cli(FILE *fp, int sockfd) {
	ssize_t n;
	char line[MAXLINE];
	struct args args;
	struct result result;

	while(Fgets(line, MAXLINE, fp) != NULL) {
		if(sscanf(line, "%ld%ld", &args.arg1, &args.arg2) != 2) {
			printf("invaild input: %s", line);
			continue;
		}
		n = strlen(line);
		Writen(sockfd, line, n);

		if((n = Readline(sockfd, line, MAXLINE)) == 0)
			err_quit("str_cli: server terminated prematurely");

		if(sscanf(line, "%ld", &result.sum) == 1)
			printf("%ld\n", result.sum);
		else
			printf("result error\n");
	}
}

int main(int argc, char **argv) {
	int 			sockfd;
	char			send[MAXLINE], recv[MAXLINE];
	FILE			*fp;
	struct sockaddr_in	saddr;

	if (argc != 2)
		err_quit("usage: ./p42echocli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &saddr.sin_addr);

	Connect(sockfd, (SA *) &saddr, sizeof(saddr));

	str_cli(stdin, sockfd);
	exit(0);
}
