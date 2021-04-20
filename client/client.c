#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define error_exit(msg) \
	do {perror(msg); exit(EXIT_FAILURE);} while(0)

#define SERV_IP "192.168.44.129"
#define SERV_PORT 5000

int main(int argc, char *argv[]) {
	int sockfd, ret;
	int n;
	char buf[BUFSIZ];
	struct sockaddr_in serv_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		error_exit("socket");

	memset(&serv_addr, 0, sizeof(serv_addr));
	ret = inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);
	if (ret <= 0) {
		if (ret == 0) 
			error_exit("format error");
		else
			error_exit("inet_pton");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);

	ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		error_exit("connect");
	}

	while(1) {
		fgets(buf, sizeof(buf), stdin);
		write(sockfd, buf, strlen(buf));
		n = read(sockfd, buf, sizeof(buf));
		write(STDOUT_FILENO, buf, n);
	}
	close(sockfd);
	
	return 0;
}
