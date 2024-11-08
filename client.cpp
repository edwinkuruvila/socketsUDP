#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

int main() {
    // Gather server details from user
    std::string portNum, hostname, message, bufferString;

    std::cout<<"Enter server hostname: ";
    std::cin>>hostname;
    std::cout<<"Enter server port number: ";
    std::cin>>portNum;
    std::cout<<"Enter a user ID: ";
    std::cin>>message;


    int sd;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // UDP socket
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        return -1;
    }

    // Configure server details
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(stoi(portNum));
    if (hostname == "localhost"){
        hostname = "127.0.0.1";
    }
    serverAddr.sin_addr.s_addr = inet_addr(hostname.c_str());
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    addr_size = sizeof(serverAddr);
    strcpy(buffer, message.c_str());

    // Send message
    sendto(sd, buffer, message.length(), 0, (struct sockaddr*)&serverAddr, addr_size);
    // If message is "Done", quit program
    if(message == "Done"){
        close(sd);
        return 0;
    }

    // Receive message
    int nBytes = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, &addr_size);
    if (nBytes < 0) {
        std::cerr << "Error receiving data!" << std::endl;
    } else {
        buffer[nBytes] = '\0';
        bufferString = buffer;

        // If server sends "None", key was not found
        if(bufferString == "None"){
            std::cout<<"The user "<<message<<" is not in the public key database."<<std::endl;
        }
        // Display found key
        else{
            std::cout<<"The public key of "<< message << " is "<<buffer<<std::endl;
        }
    }

    close(sd);
    return 0;
}
