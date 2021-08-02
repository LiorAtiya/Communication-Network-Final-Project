#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "select.hpp"
int main(int argc, char *argv[])
{
    // int listenfd = 0, listenfd2 = 0;
    // struct sockaddr_in serv_addr;
    int ret, i;
    // int r_port1 = 5000, r_port2 = 5001;
    char buff[1025];

    // listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    // listenfd2 = socket(AF_INET, SOCK_DGRAM, 0);
    // memset(&serv_addr, '0', sizeof(serv_addr));

    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // serv_addr.sin_port = htons(r_port1);

    // bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    // serv_addr.sin_port = htons(r_port2);
    // bind(listenfd2, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    int ret, i;
    char buff[1025];
    int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(5000);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);


    printf("adding fd1(%d) to monitoring\n", sockfd);
    add_fd_to_monitoring(sockfd);
    // printf("adding fd2(%d) to monitoring\n", listenfd2);
    // add_fd_to_monitoring(listenfd2);
    // listen(listenfd, 10);
    // Now server is ready to listen and verification
    // listen(listenfd, 10);
    // listen(listenfd2, 10);

    for (i = 0; i < 10; ++i)
    {
        printf("waiting for input...\n");
        ret = wait_for_input();
        printf("fd: %d is ready. reading...\n", ret);
        read(ret, buff, 1025);
        printf("\"%s\"", buff);
    }
}