#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <cstdlib>
#include <array>
#include <memory>
#define MAX_SIZE 1024

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int getPort(){
    int port;
    std::cout << "Please enter the port you want to listen: ";
    std::cin >> port;
    while(true){
        if(std::cin.fail()){
            std::cin.clear();
            std::cin.sync();
            while(std::cin.get() != '\n'){
                continue;
            }
            std::cout << "Please enter the port you want to listen: ";
            std::cin >> port;
        }
        else{
            break;
        }
    }
    return port;
}

int main(int argc, char const *argv[])
{
    int new_socket, valread, master_socket, max_sd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX_SIZE] = {0};
    char const *hello_message = "Hello client";
    int port = getPort();
    std::vector<int> client_socket;
    int max_clients = 30;
    for(int i = 0; i < max_clients; i++){
        client_socket.push_back(0);
    } 

    fd_set readfds;

    //Create a master socket
    if((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("master socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed\n");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if(bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Listener on port:" << port << std::endl;

    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(true){
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for(auto sd : client_socket){
            if(sd > 0){
                FD_SET(sd, &readfds);
            }
            if(sd > max_sd){
                max_sd = sd;
            }
        }

        //wait for an activity from one of the sockets
        auto activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if((activity < 0) && (errno != EINTR)){
            std::cout << "select error." << std::endl;
        }

        //if something happened on the master socket,
        //then it's an incoming connection
        if (FD_ISSET(master_socket, &readfds))  
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, 
                        (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            //inform user of socket number - used in send and receive commands 
            std::cout << "New connection , socket fd is " << new_socket 
                    << ", ip is: " << inet_ntoa(address.sin_addr)
                    << ", port: " << ntohs(address.sin_port) << std::endl;

            //send new connection greeting message 
            if( send(new_socket, hello_message, strlen(hello_message), 0) != strlen(hello_message) )  
            {  
                perror("send");  
            }
            std::cout << "Hello message sent" << std::endl;
            //add new socket to array of sockets
            for(int i = 0; i < max_clients; i++){
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                         
                    break;  
                }  
            }
        }

        for(int i = 0; i < max_clients; i++){
            auto sd = client_socket[i];
            if(FD_ISSET(sd, &readfds)){
                if((valread = read(sd, buffer, MAX_SIZE)) == 0){
                    //Somebody disconnected , get his details and print 
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    std::cout << "Disconnected , socket fd is " << new_socket 
                    << ", ip is: " << inet_ntoa(address.sin_addr)
                    << ", port: " << ntohs(address.sin_port) << std::endl;
                    close(sd);
                    client_socket[i] = 0;
                }
                else{
                    std::cout << valread;
                    buffer[valread] = '\0';
                    // send(sd , buffer , strlen(buffer) , 0 );
                    auto returnValue = exec(buffer);
                    memset(buffer,0,sizeof(buffer));
                    for(int i = 0; i < returnValue.length(); i++){
                        buffer[i] = returnValue[i];
                    }
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
            
        }
    }
    return 0;
}