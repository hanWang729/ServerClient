#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define MAX_SIZE 1024

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
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX_SIZE] = {0};

    int port = getPort();    

    //Create Socket
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt failed\n");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    while(1){
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // if == 0
        valread = read( new_socket , buffer, MAX_SIZE);
        printf("%s\n",buffer );
        char const *hello_message = "Hello client, your message received";
        send(new_socket , hello_message , strlen(hello_message) , 0 );
        printf("Hello message sent\n");
    }
    return 0;
}