#include "unp.h"

struct connection {
	int sock;
	char name[20];
	char ipaddr[20];
	int port;
};

int main(int argc, char **argv) {
    int 	i, j, maxi, maxfd, listenfd, connfd, sockfd;
    int 	nready;
	struct connection client[FD_SETSIZE];
    ssize_t 	n;
    fd_set 	rset, allset;
    char 	buf[MAXLINE], sendbuf[MAXLINE];
    socklen_t 	clen;
    struct sockaddr_in	caddr, saddr;
	int cnt;

	if(argc != 2)
		err_quit("usage: ./cser <PORT>");

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(atoi(argv[1]));

    Bind(listenfd, (SA *) &saddr, sizeof(saddr));
    Listen(listenfd, LISTENQ);

    /* initialize */
    maxfd = listenfd;
    maxi = -1;
	cnt = 0;
    for(i=0; i<FD_SETSIZE; i++) client[i].sock = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for( ; ; ) {
        rset = allset;
        nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		/* Add Connection */
		if(FD_ISSET(listenfd, &rset)) {
			clen = sizeof(caddr);
			connfd = Accept(listenfd, (SA *) &caddr, &clen);
	
			for(i=0; i<FD_SETSIZE; i++)
				if(client[i].sock < 0) {
					cnt++;
					client[i].sock = connfd;
					if((n = Read(connfd, client[i].name, sizeof(client[i].name))) == 0)
							err_quit("Connection's nickname missed");
					snprintf(client[i].ipaddr, sizeof(client[i].ipaddr), "%s", 
									Inet_ntop(AF_INET, &caddr.sin_addr, buf, sizeof(buf)));
					client[i].port = ntohs(caddr.sin_port);
					
					// Print Connection info to Server and Client
					sprintf(buf, "[%s] connected from %s(%d)\n", 
							client[i].name, client[i].ipaddr, client[i].port);
					printf("%s", buf);
					for(j=0; j<=maxi; j++) {
						if(client[j].sock < 0) continue;
						if(client[j].sock == connfd) continue;
						Writen(client[j].sock, buf, strlen(buf));
					}
					break;
				}
			if(i == FD_SETSIZE) err_quit("Too many clients");
			FD_SET(connfd, &allset);
			if(connfd > maxfd) maxfd = connfd;
			if(i > maxi) maxi = i;
		}

		/* STR_ECHO */
		for(i=0; i<=maxi; i++) {
			memset(sendbuf, 0, sizeof(sendbuf));
			if((sockfd = client[i].sock) < 0) continue;
			if(FD_ISSET(sockfd, &rset)) {
				if((n = Read(sockfd, buf, MAXLINE)) == 0) {
					sprintf(buf, "[%s] disconnected from %s\n", client[i].name, client[i].ipaddr);
					printf("%s", buf);
					for(j=0; j<=maxi; j++) {
						if(client[j].sock < 0) continue;
						if(client[j].sock == sockfd) continue;
						Writen(client[j].sock, buf, strlen(buf));
					}
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i].sock = -1;
					cnt--;
					memset(client[i].name, 0, sizeof(client[i].name));
					memset(client[i].ipaddr, 0, sizeof(client[i].ipaddr));
				}
				/* QUIT Command */
				else if(n == 6 && (!strncmp(buf, "/exit", 5) || !strncmp(buf, "/quit", 5) ||
									!strncmp(buf, "/EXIT", 5) || !strncmp(buf, "/QUIT", 5))) {
					sprintf(buf, "[%s] disconnected from %s\n", client[i].name, client[i].ipaddr);
					printf("%s", buf);
					for(j=0; j<=maxi; j++) {
						if(client[j].sock < 0) continue;
						if(client[j].sock == sockfd) continue;
						Writen(client[j].sock, buf, strlen(buf));
					}
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					cnt--;
					client[i].sock = -1;
					memset(client[i].name, 0, sizeof(client[i].name));
					memset(client[i].ipaddr, 0, sizeof(client[i].ipaddr));
				}
				/* LIST Command */
				else if(n == 6 && (!strncmp(buf, "/list", 5) || !strncmp(buf, "/LIST", 5))) {
					sprintf(buf, "[The number of current user is %d]\n", cnt);
					Writen(sockfd, buf, strlen(buf));
					for(j=0; j<=maxi; j++) {
						if(client[j].sock < 0) continue;
						sprintf(buf, "[%s from %s:%d]\n", client[j].name, client[j].ipaddr, client[j].port);
						Writen(sockfd, buf, strlen(buf));
					}
				}
				else if(!strncmp(buf, "/w", 2) || !strncmp(buf, "/smsg", 5)) {
					char receiver[20];
					char tmp[MAXLINE];
					char *temp;
					int chk=0;

					temp = strtok(buf, " ");
					if((temp = strtok(NULL, " ")) == NULL) {
						chk = 1;
						sprintf(sendbuf, "No receiver name\n");
						Writen(sockfd, sendbuf, strlen(sendbuf));
					} else sprintf(receiver, "%s", temp);
						
					if(chk == 0) {
						if((temp = strtok(NULL, "\n")) == NULL) {
							chk = 1;
							sprintf(sendbuf, "No mesg\n");
							Writen(sockfd, sendbuf, strlen(sendbuf));
						} else sprintf(tmp, "%s", temp);
					}
					
					if(chk == 0) {
						// 13 + name + tmp + ENTER + NULL
						n = 13 + strlen(client[i].name) + strlen(tmp) + 1 + 1;
						snprintf(sendbuf, n, "[smsg from %s] %s\n", client[i].name, tmp);

						for(j=0; j<=maxi; j++) {
							if(client[j].sock < 0) continue;
							if(client[j].sock == sockfd) continue;
							if(!strncmp(receiver, client[j].name, strlen(receiver))) {
							Writen(client[j].sock, sendbuf, n);
							}
						}
					}
				}
				else {
					// buf + name + [] + " " + NULL
					n = n + strlen(client[i].name) + 2 + 1 + 1;
					snprintf(sendbuf, n, "[%s] %s\n", client[i].name, buf);
					for(j=0; j<=maxi; j++) {
						if(client[j].sock < 0) continue;
						if(client[j].sock == sockfd) continue;
						Writen(client[j].sock, sendbuf, n);
					}
				}
				if(--nready <= 0) break;
			}
		}
	}
}
