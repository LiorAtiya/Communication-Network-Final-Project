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
#include <set>

static int message_id = 1;

using namespace std;

void print_message(string msg_id, string src_id, string dest_id, string trail_msg, string func_id, string payload)
{
    cout << "\nMSG ID: " << msg_id << " | Source ID: " << src_id << " | Destination ID: " << dest_id << " | #Trailing Msg: "
         << trail_msg << " | Function ID: " << func_id << " | Payload: " << payload << "\n"
         << endl;
}

vector<string> recieve_massage(int sockfd)
{
    char buffer[1024] = {0};
    read(sockfd, buffer, 1024);

    string segment;
    stringstream t(buffer);
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
    map<int, int> neighbors; // src: dest_id, dest_fd
    map<int, string> paths;  //src: dest_id, path
    int id;
    set<int> visited;

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
            string data = to_string(message_id++) + "," + to_string(this->id) + ",0,0,4,";
            int len = sizeof(data);
            send(sockfd, &data[0], len, 0);

            vector<string> msg_details = recieve_massage(sockfd);
            print_message(msg_details.at(0), msg_details.at(1), msg_details.at(2), msg_details.at(3), msg_details.at(4), "");

            // Stores the destination id & fd in the neighbors map
            int dest_id = stoi(msg_details.at(1));
            int dest_fd = sockfd;
            this->neighbors.insert(pair<int, int>(dest_id, dest_fd));

            cout << dest_id << endl;
            return "Ack";
        }
        return "Ack";
    }

    string Send(int dest_id, string payload)
    {
        if (neighbors.count(dest_id) != 0)
        {
            printf("sending the message\n");

            //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
            string data = to_string(message_id++) + "," + to_string(this->id) + "," + to_string(dest_id) + ",0,32," + payload;
            int len = sizeof(data);
            int sockfd = neighbors.at(dest_id);
            send(sockfd, &data[0], len, 0);

            vector<string> msg_details = recieve_massage(sockfd);
            print_message(msg_details.at(0), msg_details.at(1), msg_details.at(2), msg_details.at(3), msg_details.at(4), msg_details.at(5));
        }
        else
        {
            printf("sending failed\n");
            return "Nack";
        }
        return "Ack";
    }

    set<int> discover(int neighbor_id, int dest_id, set<int> visited)
    {
        //If visited in the neighbor node
        if (!visited.count(neighbor_id))
        {
            //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
            string data = to_string(message_id++) + "," + to_string(this->id) + "," + to_string(neighbor_id) + ",0,8," + to_string(dest_id);
            int len = sizeof(data);
            send(this->neighbors.at(neighbor_id), &data[0], len, 0);

            visited.insert(neighbor_id);
        }
        return visited;
    }

    string Route(int dest_id, set<int> visit)
    {
        if (neighbors.size() == 0)
            return "nack";

        //There is a path
        if (this->neighbors.count(dest_id))
        {
            // cout << this->paths.at(dest_id) << endl;
            return "True";
        }

        this->visited.insert(this->id);

        for (auto &pair : this->neighbors)
        {
            this->visited = discover(pair.first, dest_id, this->visited);
        }

        for (auto &pair : this->neighbors)
        {
            Route(pair.first, this->visited);
        }

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

                vector<string> msg_details = recieve_massage(connfd);
                print_message(msg_details.at(0), msg_details.at(1), msg_details.at(2), msg_details.at(3), msg_details.at(4), "");

                // Stores the destination id & fd in the neighbors map
                int dest_id = stoi(msg_details.at(1));
                int dest_fd = connfd;
                n.neighbors.insert(pair<int, int>(dest_id, dest_fd));

                //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
                string data = to_string(message_id++) + "," + to_string(n.id) + "," + to_string(dest_id) + ",0,1,";
                int len = sizeof(data);
                send(connfd, &data[0], len, 0);

                add_fd_to_monitoring(connfd);
            }
        }
        else if (ret == 0)
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
            else if (seglist.at(0) == "send")
            {
                int dest_id = stoi(seglist.at(1));
                string massage = seglist.at(3);

                cout << n.Send(dest_id, massage) << endl;
            }
            else if (seglist.at(0) == "route")
            {

                int dest_id = stoi(seglist.at(1));
                cout << n.Route(dest_id, n.visited) << endl;

                if(n.visited.count(dest_id)) { cout << "True" << endl; }
                
                for(auto x : n.visited){
                    cout << x << " ";
                }

            }
            else if (seglist.at(0) == "peers")
            {
                for (const auto &pair : n.neighbors)
                {
                    cout << "\nID: " << pair.first << " | Socket(File descriptor): " << pair.second << endl;
                }
                cout << "ack peers\n";
            }
        }
        else
        {

            vector<string> msg_details = recieve_massage(ret);
            print_message(msg_details.at(0), msg_details.at(1), msg_details.at(2), msg_details.at(3), msg_details.at(4), msg_details.at(5));

            //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
            string data = to_string(message_id++) + "," + to_string(n.id) + "," + msg_details.at(1) + ",0,1," + msg_details.at(0);
            int len = sizeof(data);
            send(ret, &data[0], len, 0);
        }
    }
}
