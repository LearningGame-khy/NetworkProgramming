#include "unp.h"

void sig_chld(int signo) {
	pid_t pid;
	int stat;

	while((pid=waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);

	return;
}

struct args {
	long arg1;
	long arg2;
};

struct result {
	long sum;
};

void str_echo(int sockfd) {
	ssize_t n;
	struct args args;
	struct result result;
	char line[MAXLINE];

	for( ; ; ) {
		if((n = Readline(sockfd, line, MAXLINE)) == 0)
			return;

		if(sscanf(line, "%ld%ld", &args.arg1, &args.arg2) == 2) {
			result.sum = args.arg1 + args.arg2;
			snprintf(line, sizeof(line), "%ld\n", result.sum);
		} else {
			snprintf(line, sizeof(line), "Input error\n");
		}
		n = strlen(line);
		Writen(sockfd, line, n);
	}
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
			str_echo(connfd);
			exit(0);
		}
		Close(connfd);
	}
}
