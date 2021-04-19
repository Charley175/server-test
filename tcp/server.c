#include "server.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>//toupper的头文件
#include <arpa/inet.h>// sockaddr_in
#include "thread_pool.h"
#include "errno.h"

void *test(void *Arg)
{
    if ( !Arg ) return NULL;
   
    uint8_t buf[1024] = {0};
  
    int client_fd = *((int *)Arg);

    while (1)
    {
        printf("%d, %x\n", client_fd, pthread_self());
        int n = read(client_fd, buf, sizeof(buf));
        if ( n <= 0 )
        {
            printf(" %x exit \n",  pthread_self());
            close(client_fd);
            return NULL;
        }
		write(client_fd, buf, n);
    }

    return NULL;
}

int main (int argc, char ** argv)
{
    if ( argc < 2 )
    {
        printf("Please Input xxx [port]\n");
        return 0;
    }

    printf("Server Port : %d\n", atoi(argv[1]));

    /*IPV4 , TCP, defult protocol*/
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if ( fd < 0 )
    {
        perror("socket");
        exit(1);
    }

	void *pool;
	int rc = threadpool_create(&pool, 1, 10, 20, 100);
	if (rc < 0) {
		printf("threadpool_create false\n");
		return -1;
	}

    struct sockaddr_in serv_addr, client_addr;
    socklen_t socklen;

    bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));  /* 指定端口号 */
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* 自动选择可用IP */

	if ( bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr) ) < 0) {
		perror("bind");
        threadpool_destroy(pool, FLAGS_WAIT_TASK_EXIT);
		return -1;
	}

	if ( listen(fd, 10) < 0 ) {
		perror("listen");
        threadpool_destroy(pool, FLAGS_WAIT_TASK_EXIT);
		return -1;
	}
    char buf[100] ={0};
    socklen = sizeof(client_addr);
    for(;;)
    {
       	int clientfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
		if (-1 == clientfd) {
			if (EINTR == errno)
				continue;
			perror("accept error.");
			return -1;
		}

        inet_ntop(AF_INET, &client_addr, buf, INET_ADDRSTRLEN);
	    printf("client IP is: %s, client port is: %d\n", buf, ntohs(client_addr.sin_port));

		rc = threadpool_add_task(pool, test, (void *)&clientfd);
		if (rc < 0) {
            threadpool_destroy(pool, FLAGS_WAIT_TASK_EXIT);
			printf("threadpool_create false\n");
			return -1;
		}

    }

	threadpool_destroy(pool, FLAGS_WAIT_TASK_EXIT);

    return 0;
}