#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define SOCK_NAME "/tmp/socket"
#define SIZE 1024

int main(int argc, char *argv[]) {

	char buffer[SIZE];
	ssize_t ret;
	/* create write socket */
	int write_sd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (write_sd == -1) {
		perror("write");
		exit(-1);
	}

	struct sockaddr_un write_un;
	
	/* init write_un */
	memset(&write_un, 0, sizeof(struct sockaddr_un));
	write_un.sun_family = AF_UNIX;
	/* copy pathname */
	strncpy(write_un.sun_path, SOCK_NAME, sizeof(write_un.sun_path) - 1);

	/* send message */
	while(1) {
		printf("input message that what you want to send: \n");
		fgets(buffer, sizeof(buffer) - 1, stdin);
		ret = sendto(write_sd, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&write_un, sizeof(struct sockaddr_un));
		/* if send data size <= 0 continue */
		if (ret == -1) {
			perror("sendto");
		}
		printf("sending data is %s\n", buffer);
	}

	/* close socket */
	close(write_sd);
	exit(0);
	
}
