#include <string.h>
#include <iostream>
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

struct Massage{
    int Msg_ID;
    int Source_ID;
    int Destination_ID;
    int Trailing_Msg;
    int Function_ID;
    string Payload; 

    void print(){
        printf("Msg_ID: %d | Source_ID: %d | Destination_ID: %d | Trailing_Msg: %d | Function_ID: %d | Payload: %s",Msg_ID, Source_ID, Destination_ID,Trailing_Msg,Function_ID,Payload);
    }
};

string setid(Node &n, int id){

    return "Ack";
}

string Ack(int msg_id){
    return "Ack\n" + msg_id; 
}

string nAck(int msg_id){
    return "nAck\n" + msg_id; 
}

string Connect(Node &n, char* dest_address, int dest_port){
    if(n.id == -1){ return "nack"; }
    
    //1.Create socket => AF_INET = ipv4 | SOCK_STREAM = TCP | 0 = Default
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        //2.create socket is faild
        printf("%s",nAck(sock));
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

    // Link the socket to the port with any IP at this port
    if (bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code ");
        close(sock);
    }

    //3. Send massage with connect, source id
    Massage msg;
    msg.Source_ID = n.id;
    msg.Destination_ID = 0;
    msg.Function_ID = 4;
    msg.Trailing_Msg = 0;
    msg.Payload = "";

    msg.print();
    //3+4 - **NEED TO SEND MSG TO DEST NODE AND GET ACK FROM THE DEST**

    return "Ack";
}

string Route(Node &n, int id){
    //The node is connected to at least one additional node
    if(n.neibhoors.size() == 0){ return "nAck"; }
}

string send(Node &n, int id, int length, string massage){
    return "Ack";
}

string peers(Node &n){
    return "Ack";
}

int main(){
    Node n;

    return 0;
}