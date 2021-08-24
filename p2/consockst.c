#include <stdio.h>
#include <netinet/in.h>

union {
	struct sockaddr_in s;
	uint8_t c[16];
} un;

void main() {
	int i;
	int port;
	char addr[16];

	printf("port: ");
	scanf("%d", &port);
	un.s.sin_port = htons(port);

	un.s.sin_family = AF_INET;
	printf("address: ");
	scanf("%s", addr);
	inet_pton(AF_INET, addr, &un.s.sin_addr);

	for (i=0; i<16; i++) {
		printf("%02x ", un.c[i]);
		if(i != 1 && i%4 == 3) printf("\n");
	}
}
