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
#include <cmath>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

using namespace std;

//-------------Utility-functions-----------------
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

// string compare_paths(string path_1, string path_2)
// {
//     string default_ans = path_1;
//     counter_1 = 1;
//     counter_2 = 1;

//     // Iterates over the first path and counts the number of nodes
//     for (int i = 0; i < path_1.length(); i++)
//     {
//         if (path_1[i] == ",")
//         {
//             counter_1++;
//         }
//     }

//     // Iterates over the second path and counts the number of nodes
//     for (int i = 0; i < path_2.length(); i++)
//     {
//         if (path_2[i] == ",")
//         {
//             counter_2++;
//         }
//     }

//     if (counter_1 > counter_2)
//     {
//         return path_1;
//     }
//     else if (counter_1 < counter_2)
//     {
//         return path_2;
//     }
//     else
//     {
//         // Iterates over the both of the paths and and compare the nodes
//         for (int i = 0; i < path_1.length(); i++)
//         {
//             if (path_1[i] > path_2[i])
//             {
//                 return path_1;
//             }
//             else if (path_1[i] > path_2[i])
//             {
//                 return path_2;
//             }
//         }
//     }

//     return ans;
// }

// string shortest_route(vector<string> routes)
// {
//     int i = 0;
//     while (routes.size() > 1)
//     {
//         string path_1 = routes.at(0);
//         routes.erase(0);
//         string path_2 = routes.at(1);
//         routes.erase(0);
//         string short_path = compare_paths(path_1, path_2);
//         routes.push_back(short_path);
//     }

//     return routes.at(0);
// }
//--------------------------------------------------

class Node
{
public:
    map<int, int> neighbors; // src: dest_id, dest_fd
    map<int, string> paths;  //src: dest_id, path
    int id;
    set<int> visited;

    string Ack(int src_id, int dest_id ,int msg_id)
    {
        string data = to_string(rand() % 100 + 1) + "," + to_string(src_id) + "," + to_string(dest_id) + ",0,1,"+to_string(msg_id);
        return data;
    }

    string Nack(int src_id, int dest_id ,int msg_id)
    {
        string data = to_string(rand() % 100 + 1) + "," + to_string(src_id) + "," + to_string(dest_id) + ",0,2,"+to_string(msg_id);
        return data;
    }

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

            //------Send connect massage------
            //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
            string data = to_string(rand() % 100 + 1) + "," + to_string(this->id) + ",0,0,4,";
            int len = sizeof(data);
            send(sockfd, &data[0], len, 0);

            //------Recive ack massage------
            vector<string> msg_details = recieve_massage(sockfd);
            print_message(msg_details.at(0), msg_details.at(1), msg_details.at(2), msg_details.at(3), msg_details.at(4), msg_details.at(5));

            // Stores the destination id & fd in the neighbors map
            int dest_id = stoi(msg_details.at(1));
            int dest_fd = sockfd;
            this->neighbors.insert(pair<int, int>(dest_id, dest_fd));
            string path =  to_string(this->id) + "->" + to_string(dest_id);
            this->paths.insert(pair<int, string>(dest_id, path));
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
            int MAX_SIZE = 488;
            int trailing = round(sizeof(payload) / MAX_SIZE);
            cout << "trailing: " << trailing << endl;
            vector<string> payloads;

            if (sizeof(payload) > MAX_SIZE)
            {
                int temp_size = sizeof(payload);
                int i = 0;
                while (temp_size > 0)
                {
                    string temp = "";
                    while (i % MAX_SIZE < MAX_SIZE)
                    {
                        temp[i] += payload[i];
                        temp_size--;
                        i++;
                    }
                    payloads.push_back(temp);
                }
            }

            else
            {
                payloads.push_back(payload);
            }

            int j = 0;
            while (trailing >= 0)
            {
                //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
                cout << "payloads: " << payloads.at(j++) << endl;
                string data = to_string(rand() % 100 + 1) + "," + to_string(this->id) + "," + to_string(dest_id) +
                              "," + to_string(trailing--) + ",32," + payloads.at(j++);
                int len = sizeof(data);
                int sockfd = neighbors.at(dest_id);
                send(sockfd, &data[0], len, 0);

                vector<string> msg_details = recieve_massage(sockfd);
                print_message(msg_details.at(0), msg_details.at(1), msg_details.at(2), msg_details.at(3), msg_details.at(4), msg_details.at(5));
            }
        }
        else
        {
            printf("sending failed\n");
            return "Nack";
        }
        return "Ack";
    }

    string FindPaths(int dest_id)
    {
        //Already pathd is calculated
        if(this->paths.count(dest_id) != 0){
            cout << "ack\n";
            return this->paths.at(dest_id);
        }

        int count_nack = 0;
        vector<string> all_paths;
        // iterates over all the neighbours of the node
        for (const auto &node : this->neighbors)
        {
            // int prev_id = this->id;
            int current_id = node.first;
            int sockfd = node.second;
            string ans = discover(dest_id, sockfd);
            if(ans == "nack"){
                count_nack++;
            }else{
                all_paths.push_back(ans);
            }
        }
        //If all return nack - don't have paths
        if(count_nack == this->neighbors.size()){
            return "nack";
        }

        //The shortest path
        return all_paths.at(0);
    }

    string discover(int neighbor_id, int dest_id)
    {
        if (neighbor_id != dest_id)
        {
            //Neighbors of neighbor node
            for(auto &pair : this->neighbors){
                //-----Send discover massage------
                //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
                string data = to_string(rand() % 100 + 1) + "," + to_string(this->id) + "," + to_string(neighbor_id) + ",0,8," + to_string(dest_id);
                int len = sizeof(data);
                int sockfd = pair.second;
                send(sockfd, &data[0], len, 0);

                vector<string> msg_details = recieve_massage(sockfd);
                //If src id = dest id
                if(msg_details.at(1) == msg_details.at(2)){
                    return "Have a path";
                }else{
                    discover(pair.first, dest_id);
                }
                
            }
        } else{
            return "Have a path";
        }
        return "nack";
    }

    // string discover(int prev_id, int dest_id, int current_id, int sockfd)
    // {
    //     if (current_id == dest_id) // i am the target node
    //     {
    //         cout << "ack" << endl;
    //         Route(current_id);
    //     }

    //     else // i am not the target node
    //     {
    //         // iterates over all the neighbours of the node
    //         for (const auto &node : this->neighbors)
    //         {
    //             if (node != prev_id) // if the neighbor is not the previous node
    //             {
    //                 int new_prev_id = this->id;
    //                 int new_current_id = node.first;
    //                 int new_sockfd = node.second;
    //                 discover(new_prev_id, dest_id, new_current_id, new_sockfd);
    //             }
    //         }
    //     }

    //     return "nack";
    // }

    // string Route(int dest_id)
    // {
    //     vector<string> routes; // a vector that contains all the paths
    //     string short_path = shortest_route(routes);

    //     return short_path;
    // }
};

int main(int argc, char *argv[])
{
    /* initialize random seed: */
    srand (time(NULL));

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

                //------Recieve connect massage------
                vector<string> msg_details = recieve_massage(connfd);
                print_message(msg_details.at(0), msg_details.at(1), msg_details.at(2), msg_details.at(3), msg_details.at(4), "");

                // Stores the destination id & fd in the neighbors map
                int dest_id = stoi(msg_details.at(1));
                int dest_fd = connfd;
                n.neighbors.insert(pair<int, int>(dest_id, dest_fd));
                string path = to_string(n.id) + "->" + to_string(dest_id);
                n.paths.insert(pair<int, string>(dest_id, path));

                //------Send Ack massage------
                //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
                int msg_id = stoi(msg_details.at(0));
                string data = n.Ack(n.id,dest_id ,msg_id);
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
                cout << n.FindPaths(dest_id) << endl;

            }
            else if (seglist.at(0) == "peers")
            {
                cout << "ack\n";
                for (const auto &pair : n.neighbors)
                {
                    cout << pair.first << ",";
                }
                cout << "\n";
                
            }
        }
        else
        {
            vector<string> msg_details = recieve_massage(ret);
            print_message(msg_details.at(0), msg_details.at(1), msg_details.at(2), msg_details.at(3), msg_details.at(4), msg_details.at(5));

            //Data = Msg_ID | Src_ID | Dest_ID | # Trailing Msg | Function ID | Payload
            string data = to_string(rand() % 100 + 1) + "," + to_string(n.id) + "," + msg_details.at(1) + ",0,1," + msg_details.at(0);
            int len = sizeof(data);
            send(ret, &data[0], len, 0);
        }
    }
}
