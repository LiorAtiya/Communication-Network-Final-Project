#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

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
    const char* Payload = new char[492];

    void print()
    {
        printf("Msg_ID: %d | Source_ID: %d | Destination_ID: %d | Trailing_Msg: %d | Function_ID: %d | Payload: %s\n", Msg_ID, Source_ID, Destination_ID, Trailing_Msg, Function_ID, Payload);
    }
};

string Ack(int msg_id)
{
    return "Ack\n" + msg_id;
}

string nAck(int msg_id)
{
    return "nAck\n" + msg_id;
}

string Send(int sock, Massage msg)
{
    if (send(sock, &msg, sizeof(msg), 0) == -1)
    {
      return "nack";
    }
    return "ack";
}

string Connect(Node &n, const char *dest_address, int dest_port)
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
        cout << nAck(sock) << endl;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //Contains information about the IP of the socket for different types of sockets
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(dest_port); //network order
    int rval = inet_pton(AF_INET, dest_address, &serverAddress.sin_addr);
    if (rval <= 0)
    {
        printf("inet_pton() failed");
    }

    //Make a connection to the server with socket SendingSocket.
    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("connect() failed with error code : %d\n", errno);
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
    // Send(sock, msg);
    // char buffer[492];
    // //Wait for ack from the destination node
    // if(recv(sock, buffer, 492,0) == -1){
    //     return "nack";
    // }

    msg.print();
    //3+4 - **NEED TO SEND MSG TO DEST NODE AND GET ACK FROM THE DEST**

    return "ack2";
}

// string peers(Node &n)
// {
//     return "Ack";
// }

int main()
{
    Node n;
    string text;
    string segment;

    // while(true){

    //     vector<string> seglist;
    //     cin >> text;
    //     stringstream t(text);
    //     while(getline(t, segment, ',')){
    //         seglist.push_back(segment);
    //     }
    //     if(seglist.at(0) == "setid"){
    //         n.id = stoi(seglist.at(1));
    //         cout << "ack1" << endl;
    //     }else if(seglist.at(0) == "connect"){
    //         const char* ip_address = seglist.at(1).c_str();
    //         int port_address = stoi(seglist.at(2));
    //         cout << Connect(n, ip_address, port_address) << endl;

    //         // cout << ip_address << endl;
    //         // cout << port_address << endl;
    //     }else if(seglist.at(0) == "send"){

    //         int id = stoi(seglist.at(1));
    //         int len = stoi(seglist.at(2));
    //         string massage = seglist.at(3);

    //     }else if(seglist.at(0) == "route"){

    //         int id = stoi(seglist.at(1));

    //     }else if(seglist.at(0) == "Peers"){

    //     }
    // }


    
    

    return 0;
}