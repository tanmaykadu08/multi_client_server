#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void receiveMessages(int socket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));                         // Clear the buffer before receiving new data
        int bytes = recv(socket, buffer, sizeof(buffer), 0); // Receive data from the server

        if (bytes <= 0)
            break; // If server disconnects or an error occurs, break the loop

        std::cout << "Message: " << buffer << std::endl;


    }
}


int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock

    // Create a TCP socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0); 
    
    // Define the server address structure
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // Zero out the structure
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = htons(8080); // Connect to port 8080

    // Connect to the server
    connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    std::cout << "Connected to the server.\n";

    // Start a thread to receive messages from the server
    std::thread receiveThread(receiveMessages, clientSocket); 

    // main loop
    while(true)
    {
        std::string message;
        std::cout << "Enter from your side: ";
        std::getline(std::cin, message); // Read user input

        if (message == "exit")
        {
            std::cout << "Disconnecting...\n";
            break; // Exit the loop if the user types "exit"
        }

        send(clientSocket, message.c_str(), message.length(), 0); // Send the message to the server
    }
    receiveThread.join(); // Wait for the receive thread to finish

    // Clean up
    closesocket(clientSocket); // Close the client socket
    WSACleanup(); // Clean up Winsock


    return 0;
}