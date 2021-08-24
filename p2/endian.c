#include <stdio.h>
#include <netinet/in.h>

union {
	uint32_t i;
	uint8_t c[4];
} un;

void main() {
	int i;

	scanf("%x", &un.i);	
	for(i=0; i<4; i++)	
		printf("%x", un.c[i]);
	printf("\n");
}
