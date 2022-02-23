// Client side C/C++ program to demonstrate Socket programming
#include <iostream>
#include <regex>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
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

std::string getAddress(){
    std::string addr;
    std::cout << "Please input the ip address:";
    std::cin >> addr;
    std::regex pattern("^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$");
    while(true){
        if(!std::regex_match(addr,pattern)){
            std::cin.clear();
            std::cin.sync();
            while(std::cin.get() != '\n'){
                continue;
            }
            std::cout << "Please enter the port you want to listen: ";
            std::cin >> addr;
        }
        else{
            break;
        }
    }
    return addr;
}

int main(int argc, char const *argv[])
{
    int port = getPort();
    std::string addr = getAddress();

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char send_buffer[MAX_SIZE] = {0};
    char recv_buffer[MAX_SIZE] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    // Remote VM 10.210.154.207
    // Citirx 10.63.61.150
    if(inet_pton(AF_INET, addr.c_str(), &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    std::string message;
    std::cout << "Input the message you want to sent to server:" << std::endl;
    while(true){
        valread = read( sock , recv_buffer, 1024);
        printf("%s\n",recv_buffer);
        std::getline(std::cin, message, ';');
        if(message == "exit"){
            shutdown(sock, SHUT_WR);
            close(sock);
            break;
        }
        strcpy(send_buffer, message.c_str());
        send(sock , send_buffer , strlen(send_buffer) , 0 );
        memset(recv_buffer,0,sizeof(recv_buffer));
        memset(send_buffer,0,sizeof(send_buffer));       
    }
    return 0;
}