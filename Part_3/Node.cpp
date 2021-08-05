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

using namespace std;

void print_message(int msg_id, int src_id, int dest_id, int trail_msg, int func_id, string payload)
{
    cout << "\nMSG ID: " << msg_id << " | Source ID: " << src_id << " | Destination ID: " << dest_id << " | #Trailing Msg: "
         << trail_msg << " | Function ID: " << func_id << " | Payload: " << payload << "\n"
         << endl;
}

vector<string> recive_massage(int sockfd)
{
    uint32_t dataLength;
    std::vector<uint8_t> rcvBuf;     // Allocate a receive buffer
    rcvBuf.resize(dataLength, 0x00); // with the necessary size

    recv(sockfd, &(rcvBuf[0]), dataLength, 0); // Receive the string data
    string data_recv = "";
    for (int i = 0; i < rcvBuf.size(); i++)
    {
        data_recv += rcvBuf.at(i);
        cout << rcvBuf.at(i);
    }

    string segment;
    stringstream t(data_recv);
    vector<string> msg_details;

    while (getline(t, segment, ','))
    {
        msg_details.push_back(segment);
    }
    return msg_details;
}

class Node
{
public:
    map<int, int> neighbors;
    int id;

    string Connect(string ip, int port)
    {
        int sockfd;
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
        {
            printf("connected to the server..\n");

            //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
            string data = "555," + to_string(this->id) + ",0,0,4,";
            send(sockfd, data.data(), data.size(), 0);

            vector<string> msg_details = recive_massage(sockfd);
            // print_message(stoi(msg_details.at(0)), stoi(msg_details.at(1)), stoi(msg_details.at(2)), stoi(msg_details.at(3)), stoi(msg_details.at(4)), "");
        }

        return "ack";
    }

    string Ack(int numOfMsg)
    {
        return "Ack";
    }

    string Nack(int numOfMsg)
    {
        return "Nack";
    }
    // bool Send(int len, Message msg);
    // void Route();
    // string receive(int);
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

    while (true)
    {
        char buff[1025];
        printf("waiting for input...\n");
        //return the fd
        ret = wait_for_input();
        //Accept connection from another socket
        if (ret == listenfd)
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

                vector<string> msg_details = recive_massage(connfd);
                // print_message(stoi(msg_details.at(0)), stoi(msg_details.at(1)), stoi(msg_details.at(2)), stoi(msg_details.at(3)), stoi(msg_details.at(4)), "");
                // Enter to the neibhoors
                // int port = htons(cli.sin_port);
                // n.neighbors.insert({stoi(msg_details.at(1)), connfd});

                //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
                string data = "555," + to_string(n.id) + ",0,0,4,";
                send(connfd, data.data(), data.size(), 0);

                // add_fd_to_monitoring(connfd);
                printf("waiting for input...\n");
                ret = wait_for_input();
            }
        }
        else if (ret != 0)
        {
            // vector<string> msg_details = recive_massage(ret);
            // print_message(stoi(msg_details.at(0)), stoi(msg_details.at(1)), stoi(msg_details.at(2)), stoi(msg_details.at(3)), stoi(msg_details.at(4)), "");
        }
        //Read from the command line
        else
        {
            printf("fd: %d is ready. reading...\n", ret);
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
            else if (seglist.at(0) == "peers")
            {
                for (const auto &pair : n.neighbors)
                {
                    cout << "\nID: " << pair.first << " | Socket(File descriptor): " << pair.second << endl;
                }
                cout << "ack peers\n";
            }
            bzero(buff, 1025);
        }
    }
}