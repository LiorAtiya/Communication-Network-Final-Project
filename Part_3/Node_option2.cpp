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

#define IP_ADDRESS "10.0.2.15"
using namespace std;

struct Node
{
    int id = -1;
    string Source_ID;
    string Destination_ID;
    string port;
    vector<Node> neibhoors;
};

struct Massage
{
    int Msg_ID;
    int Source_ID;
    int Destination_ID;
    int Trailing_Msg;
    int Function_ID;
    const char *Payload = new char[492];

    void print()
    {
        printf("Msg_ID: %d | Source_ID: %d | Destination_ID: %d | Trailing_Msg: %d | Function_ID: %d | Payload: %s\n", Msg_ID, Source_ID, Destination_ID, Trailing_Msg, Function_ID, Payload);
    }
};

string Connect(Node &n, string dest_address, int dest_port)
{
    if (n.id == -1)
    {
        return "nack";
    }

    //1.Create socket => AF_INET = ipv4 | SOCK_STREAM = TCP | 0 = Default
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        //2.create socket is faild
        // printf("%s", nAck(sock));
        cout << "nack" << endl;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //Contains information about the IP of the socket for different types of sockets
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(dest_port); //network order
    int rval = inet_pton(AF_INET, (const char *)IP_ADDRESS, &serverAddress.sin_addr);
    if (rval <= 0)
    {
        printf("inet_pton() failed");
    }

    //Make a connection to the server with socket SendingSocket.
    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("connect() failed with error code : %d\n", errno);
        return "nack";
        //2.create connect is faild
    }

    //3. Send massage with connect, source id
    Massage msg;
    msg.Source_ID = n.id;
    msg.Destination_ID = 0;
    msg.Function_ID = 4;
    msg.Trailing_Msg = 0;
    string s = "connect";
    msg.Payload = s.c_str();

    // //Send massage to destination node


    msg.print();
    //3+4 - **NEED TO SEND MSG TO DEST NODE AND GET ACK FROM THE DEST**

    return "ack2";
}

int main(int argc, char *argv[])
{
    int listenfd = 0;
    struct sockaddr_in serv_addr;
    int ret, i;
    int port;
    cout << "Enter port to this node: ";
    cin >> port;
    char buff[1025];
    time_t ticks;

    //Listen to this node
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if(bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        printf("Bind failed with error code ");
        close(listenfd);
        return -1;
    }

    serv_addr.sin_port = htons(port);

    printf("adding fd1(%d) to monitoring\n", listenfd);
    add_fd_to_monitoring(listenfd);
    listen(listenfd, 100);

    // struct sockaddr_in clientAddress;
    // socklen_t clientAddressLen = sizeof(clientAddress);
    //  //3. Accept and incoming connection
    // //request for connection from the queue of requests
    // int clientSocket = accept(listenfd, (struct sockaddr *)&clientAddress, &clientAddressLen);
    // if (clientSocket == -1)
    // {
    //     printf("listen failed with error code ");
    //     return -1;
    // }

    Node n;
    string text;

    while (true)
    {
        printf("waiting for input...\n");
        ret = wait_for_input();
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
            cout << "ack1" << endl;
        }
        else if (seglist.at(0) == "connect")
        {
            string ip_address = seglist.at(1).c_str();
            int port_address = stoi(seglist.at(2));
            cout << Connect(n, ip_address, port_address) << endl;

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
}