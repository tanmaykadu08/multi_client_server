#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

std::vector<int> clients; // Vector to store client sockets

//function to handle client communication
void handleClient(int clientSocket) {
    char buffer[1024];

    while (true)
     {
        memset(buffer, 0, sizeof(buffer));//clear the buffer before receiving new data
        int bytes= recv(clientSocket, buffer, sizeof(buffer), 0);// Receive data from the client
       
        if (bytes <= 0) break; //if client disconnects or an error occurs, break the loop 
   

        std::cout << "Received from client " << clientSocket << ": " << buffer << std::endl;

        //send the received message back to the client (echo)
        for(int sock  : clients)
         {
         
             send(sock, buffer, strlen(buffer), 0); // Echo back the message
        }
    }

    // Remove the client socket from the vector and close it
    closesocket(clientSocket);
    clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());// Remove the client socket from the vector
    std::cout << "Client " << clientSocket << " disconnected.\n" << std::endl;
}


int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // Zero out the structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080); // Bind to port 8080

    // Bind the socket to the address and port
    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); 

    // Listen for incoming connections
    listen(serverSocket, SOMAXCONN); //!SOMAXCONN is a constant that specifies the maximum length of the queue of pending connections

    std::cout << "Server is listening on port 8080...\n";

    while (true)
    {
        //accept a new client connection
        int addrLen = sizeof(serverAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&serverAddr, &addrLen); // Accept a new client connection
        
        std::cout << "Client " << clientSocket << " connected.\n";

        clients.push_back(clientSocket); // Add the new client socket to the vector

        // Create a new thread to handle the client communication
        std::thread t(handleClient, clientSocket);
        t.detach(); // Detach the thread to allow it to run independently

    }
    // Cleanup Winsock (this code will never be reached in this example, but it's good practice)
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}