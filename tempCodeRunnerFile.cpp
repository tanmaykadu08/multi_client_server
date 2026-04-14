#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<chrono>
#include<mutex>
#pragma comment(lib, "ws2_32.lib")

std::vector<int> clients; // Vector to store client sockets
std::mutex clientsMutex; // Mutex to protect access to the clients vector

//function to handle client communication
void handleClient(int clientSocket) {
    char buffer[1024];

    while (true)
     {
        memset(buffer, 0, sizeof(buffer));//clear the buffer before receiving new data
        int bytes= recv(clientSocket, buffer, sizeof(buffer), 0);// Receive data from the client
       
        if (bytes <= 0) break; //if client disconnects or an error occurs, break the loop 
   

        std::cout << "Received from client " << clientSocket << ": " << buffer << std::endl;

        //lock mutex before accessing the clients vector
        {        std::lock_guard<std::mutex> lock(clientsMutex);
            //send the received message back to the client (echo)
            for(int sock  : clients)
            {
                if(sock != clientSocket) // Don't send the message back to the sender
                {
                    send(sock, buffer, strlen(buffer), 0); // Echo back the message to other clients
                }
            
            }
        }
    }

    //lock mutex before modifying the clients vector
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());// Remove the client socket from the vector
    }
    // Remove the client socket from the vector and close it
    closesocket(clientSocket);
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
        //lock mutex before modifying the clients vector
        {
            std::lock_guard<std::mutex> lock(clientsMutex); // Lock the mutex before modifying the clients vector
            clients.push_back(clientSocket); // Add the new client socket to the vector
        }

        // Create a new thread to handle the client communication
        std::thread t(handleClient, clientSocket);
        t.detach(); // Detach the thread to allow it to run independently

    }
    // Cleanup Winsock (this code will never be reached in this example, but it's good practice)
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}