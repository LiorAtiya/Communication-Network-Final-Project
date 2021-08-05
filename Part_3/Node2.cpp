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
#include <map>

static int message_id = 1;

using namespace std;

class Node
{
public:
    map<int, int> neighbors; // src: dest_port, dest_port
    int id;

    string Connect(string ip, int port)
    {
        int sockfd;
        struct sockaddr_in servaddr;

        // socket create and varification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            printf("socket creation failed...\n");
            return "Nack";
        }
        else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(port);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return "Nack";
        }

        // connect the client socket to server socket
        if (connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0)
        {
            printf("connection with the server failed...\n");
            return "Nack";
        }
        else
        {
            printf("connected to the server..\n");

            //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
            string data = to_string(message_id++) + "," + to_string(this->id) + ",0,0,4,\n";
            int len = sizeof(data);
            send(sockfd, &data[0], len, 0);

            char buffer[1024] = {0};
            read(sockfd, buffer, 1024);
            printf("%s\n", buffer);

            // Stores the destination id
            string segment;
            stringstream t(buffer);
            vector<string> seglist;
            while (getline(t, segment, ','))
            {
                seglist.push_back(segment);
            }
            auto dest_id = stoi(seglist.at(1));

            // inserts the connection to the neighbors map
            neighbors.insert(pair<int, int>(dest_id, port));

            // Prints the neighbors map
            for (auto const &pair : neighbors)
            {
                std::cout << "{" << pair.first << ": " << pair.second << "}\n";
            }

            // vector<string> msg_details = recive_massage(sockfd);
            // print_message(stoi(msg_details.at(0)), stoi(msg_details.at(1)), stoi(msg_details.at(2)), stoi(msg_details.at(3)), stoi(msg_details.at(4)), "");
        }
        return "ack";
    }

    string Send(int port, string msg)
    {

        return "ack";
    }

};

int main(int argc, char *argv[])
{

    int listenfd = 0;
    int ret, i;
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
    if ((listen(listenfd, 10)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    printf("adding fd1(%d) to monitoring\n", listenfd);
    add_fd_to_monitoring(listenfd);

    Node n;

    while (true)
    {

        printf("waiting for input...\n");
        //return the fd
        ret = wait_for_input();
        //Accept connection from another socket
        if (ret != 0)
        {
            printf("fd: %d is ready. reading...\n", ret);
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

                char buffer[1024] = {0};
                read(connfd, buffer, 1024);
                printf("%s\n", buffer);

                // Stores the new destination id
                string segment;
                stringstream t(buffer);
                vector<string> seglist;
                while (getline(t, segment, ','))
                {
                    seglist.push_back(segment);
                }
                auto dest_id = stoi(seglist.at(1));

                //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
                string data = to_string(message_id++) + "," + to_string(n.id) + "," + to_string(dest_id) + ",0,4,\n";
                int len = sizeof(data);
                send(connfd, &data[0], len, 0);

                add_fd_to_monitoring(connfd);

                printf("waiting for input...\n");
                ret = wait_for_input();
            }
        }
        else
        {
            char buff[1025];
            printf("fd: %d is ready. reading (from keyboard)...\n", ret);
            read(ret, buff, 1025);

            string segment;
            stringstream t(buff);
            vector<string> seglist;

            while (getline(t, segment, ','))
            {
                seglist.push_back(segment);
            }

            if (seglist.at(0) == "setid")
            {
                n.id = stoi(seglist.at(1));
                cout << "ack\n";
            }
            else if (seglist.at(0) == "connect")
            {
                string ip_address = seglist.at(1).c_str();
                int port_address = stoi(seglist.at(2));
                cout << n.Connect(ip_address, port_address) << endl;
            }
        }
    }
}
