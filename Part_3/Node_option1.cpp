/*
    C++ Socket Client
*/
#include <iostream> //cout
#include <stdio.h> //printf
#include <string.h> //strlen
#include <string> //string
#include <sys/socket.h> //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include <vector>

#include "select.hpp"
using namespace std;

struct Message
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

class Node
{
    private:
        int sock;
        int id;
        string address;
        string response_data = "";
        int port;
        struct sockaddr_in server;
        vector<Node> neibhoors;

    public:
        Node(int p);
        string Connect(string, int);
        bool Send(int len, Message msg);
        void Route();
        string receive(int);
};

/*
    constructor
*/
Node::Node(int p)
{
    sock = -1;
    port = p;
    address = "10.0.2.15";
}

/*
    Connect to a host on a certain port number
*/
string Node::Connect(string address , int port)
{
    // create socket if it is not already created
    if(sock == -1)
    {
        //1. Create TCP socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
            //2.Faild
            return "nack";
        }

        cout<<"Socket created\n";
    }
    else { /* OK , nothing */ }

    // setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;

        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";

            return "nack";
        }

        // Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];

            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

            break;
        }
    }

    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    //Connect to remote server
    if( connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0 )
    {
        perror("connect failed. Error");
        return "nack";
    }

    cout<<"Connected\n";

    //3. Send massage with connect, source id
    Message msg;
    msg.Source_ID = this->id;
    msg.Destination_ID = 0;
    msg.Function_ID = 4;
    msg.Trailing_Msg = 0;
    string s = "connect";
    msg.Payload = s.c_str();

    Send(sizeof(msg), msg);

    // string ans = receive();
    // if(currect details){
    //     this->neibhoors(id);
    // }
    // printf(id);

    return "ack";
}

/*
    Send data to the connected host
*/
bool Node::Send(int dataLen, Message msg)
{
    // cout<<"Sending data...";
    // cout<<data;
    // cout<<"\n";
    
    // Send some data
    if( send(sock , &msg , dataLen , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    
    cout<<"Data send\n";

    return true;
}

/*
    Receive data from the connected host
*/
string Node::receive(int size=512)
{
    char buffer[size];
    string reply;

    //Receive a reply from the server
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        puts("recv failed");
        return NULL;
    }

    reply = buffer;
    response_data = reply;
    
    return reply;
}

// void Route(int num_discover, int ansLen, ){

// }

int main(int argc , char *argv[])
{
    //---------------Taken from example.cpp------------------
    int listenfd = 0;
    struct sockaddr_in serv_addr;
    int ret, i;
    int this_port;
    cout << "Enter port to listen this node: ";
    cin >> this_port;
    char buff[1025];
    time_t ticks;

    //Listen to this node
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    cout << "listenfd: " << listenfd << endl;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(this_port);

    if(bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        printf("Bind failed with error code ");
        return -1;
    }

    printf("adding fd1(%d) to monitoring\n", listenfd);
    add_fd_to_monitoring(listenfd);
    listen(listenfd, 100);
    //--------------------------------------------------

    while(true){

        printf("waiting for input...\n");
	    ret = wait_for_input();
        struct sockaddr_in clientAddress; //
        socklen_t clientAddressLen = sizeof(clientAddress);

        accept(listenfd, (struct sockaddr *)&clientAddress, &clientAddressLen);

	    // printf("fd: %d is ready. reading...\n", ret);
	    // read(ret, buff, 1025);

        // int init_port;
        // cout <<"Enter node port: ";
        // cin>>init_port;
        Node c(5);
        string host;
        int port;

        //For Connect
        cout << "Connect to another node: " << endl;
        cout<<"Enter hostname : ";
        cin>>host;
        cout <<"Enter port: ";
        cin>>port;

        //connect to host
        c.Connect(host , port);

    }

    // //send some data
    // c.send_data("GET / HTTP/1.1\r\n\r\n");

    // //receive and echo reply
    // cout<<"----------------------------\n\n";
    // cout<<c.receive(1024);
    // cout<<"\n\n----------------------------\n\n";

    //done
    return 0;
}