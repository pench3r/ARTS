#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#define SOCKET_NAME "/tmp/socket"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {

	char buffer[BUFFER_SIZE];

	/* create file system socket */
	int read_sd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (read_sd == -1) {
		perror("create socket failed!");
		exit(-1);
	}
	
	unlink(SOCKET_NAME);

	struct sockaddr_un read_un;
	memset(&read_un, 0, sizeof(struct sockaddr_un));

	read_un.sun_family = AF_UNIX;
	strncpy(read_un.sun_path, SOCKET_NAME, sizeof(read_un.sun_path) - 1);

	/* bind file to socket */
	int ret = bind(read_sd, (const struct sockaddr *)&read_un, sizeof(struct sockaddr_un));
	if (ret == -1) {
		perror("bind");
		exit(-1);
	}	
	printf("[*] Bind...\n");

	/* begin to recvfrom data from socket */
	struct sockaddr_un client_un;
	socklen_t client_ul = sizeof(client_un);
	while(1) {
		ret = recvfrom(read_sd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_un, &client_ul);
		/* if ret le 0 continue */
		if ( ret == -1 ) {
			fprintf(stderr, "%s\n", strerror(errno));	
		}
		printf("recv data is %s.\n", buffer);	
	}
	/* close socket */
	close(read_sd);

	/* unlink socket file */
	unlink(SOCKET_NAME);
	exit(0);
}
