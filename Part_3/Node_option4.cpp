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

#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class Node
{
private:
    int sock;
    string address;
    string response_data = "";
    int port;
    struct sockaddr_in server;
    vector<Node> neibhoors;

public:
    int id;
    string Connect(string ip, int port)
    {
        int sockfd, connfd;
        struct sockaddr_in servaddr, cli;

        // socket create and varification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            printf("socket creation failed...\n");
            exit(0);
        }
        else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(port);

        // connect the client socket to server socket
        if (connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0)
        {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
            printf("connected to the server..\n");

        return "ack";
    }
    // bool Send(int len, Message msg);
    // void Route();
    // string receive(int);
};

int main(int argc, char *argv[])
{
    int listenfd = 0;
    int ret, i;
    char buff[1025];
    int connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    // Binding newly created socket to given IP and verification
    if ((bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(listenfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    printf("adding fd1(%d) to monitoring\n", listenfd);
    add_fd_to_monitoring(listenfd);

    Node n;
    int e = 0;
    while (true)
    {
        e++;
        printf("waiting for input...\n");
        //return the fd
        ret = wait_for_input();
        //Recive from any another socket
        if (ret != 0)
        {
            struct sockaddr_in cli;
            socklen_t len = sizeof(cli);

            // Accept the data packet from client and verification
            connfd = accept(ret, (sockaddr *)&cli, &len);
            if (connfd < 0)
            {
                printf("server acccept failed...\n");
                exit(0);
            }
            else
            {
                printf("server acccept the client...\n");
                read(connfd, buff, 1025);
            }
        }else{
            read(ret, buff, 1025);
        }
        // printf("fd: %d is ready. reading...\n", ret);

        string segment;
        stringstream t(buff);
        vector<string> seglist;

        while (getline(t, segment, ','))
        {
            seglist.push_back(segment);
        }

        if (seglist.size() > 0)
        {
            if (seglist.at(0) == "setid")
            {
                n.id = stoi(seglist.at(1));
                cout << "ack" << endl;
            }
            else if (seglist.at(0) == "connect")
            {
                string ip_address = seglist.at(1).c_str();
                int port_address = stoi(seglist.at(2));
                cout << n.Connect(ip_address, port_address) << endl;

                // cout << ip_address << endl;
                // cout << port_address << endl;
            }
            else if (seglist.at(0) == "send")
            {

                int id = stoi(seglist.at(1));
                int len = stoi(seglist.at(2));
                string massage = seglist.at(3);
            }
            else if (seglist.at(0) == "route")
            {

                int id = stoi(seglist.at(1));
            }
            else if (seglist.at(0) == "Peers")
            {
            }
        }
        else
        {
        }

        // printf("\"%s\"", buff);
    }
}