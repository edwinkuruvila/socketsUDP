#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <fstream>

// Stores private and public keys in vectors
std::vector<std::string> publicKeys;
std::vector<std::string> privateKeys;

// Read from keys file and store in vector
void getPresentKeys(){
    std::string lines, firstHalf, secondHalf, filename;
    int space;

    std::cout<<"Enter a file name: ";
    std::cin>>filename;

    std::ifstream inputfile(filename);
    while(getline (inputfile, lines)){
        space = lines.find(" ");
        if(space == -1){
            break;
        }
        firstHalf = lines.substr(0, space);
        secondHalf = lines.substr(space, lines.size()-space);
        remove(secondHalf.begin(), secondHalf.end(), ' ');
        publicKeys.push_back(firstHalf);
        privateKeys.push_back(secondHalf);
    }

    inputfile.close();
}


int main() {
    std::string portNum, message, bufferString;
    int sd, iter;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    // Get input from console
    getPresentKeys();
    std::cout<<"Enter server port number: ";
    std::cin>>portNum;

    // UDP socket
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        return -1;
    }

    // Configure server details 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(stoi(portNum));
    serverAddr.sin_addr.s_addr = INADDR_ANY;  
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    // Bind socket
    if (bind(sd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        return -1;
    }

    // Receive message
    addr_size = sizeof(clientAddr);
    while (true) {
        int nBytes = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &addr_size);
        if (nBytes < 0) {
            std::cerr << "Error receiving data!" << std::endl;
            continue;
        }

        buffer[nBytes] = '\0';
        bufferString = buffer;

        // Send message
        // If message recieved is "Done", close server
        if(bufferString == "Done"){
            std::cout<<"Terminated by user request."<<std::endl;
            return 0;
        }

        // Interate through vectors and see if name is found
        iter = -1;
        for(int i = 0; i<publicKeys.size(); i++){
            if(publicKeys.at(i) == bufferString){
                iter = i;
            }
        }

        // If name is not found, return None
        if(iter==-1){
            std::cout<<buffer<<": Not in the database."<<std::endl;
            message = "None";
            strcpy(buffer, message.c_str());
            sendto(sd, buffer, message.length(), 0, (struct sockaddr*)&clientAddr, addr_size);
        }
        //  If name is found, send private key
        else if(iter>-1){
            std::cout<<buffer<<": Found a public key."<<std::endl;
            message = privateKeys.at(iter);
            strcpy(buffer, message.c_str());
            sendto(sd, buffer, message.length(), 0, (struct sockaddr*)&clientAddr, addr_size);
        }
    }

    return 0;
}
