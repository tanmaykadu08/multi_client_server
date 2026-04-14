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

std::mutex coutMutex; // Mutex to protect access to std::cout

void receiveMessages(int socket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));                         // Clear the buffer before receiving new data
        int bytes = recv(socket, buffer, sizeof(buffer), 0); // Receive data from the server

        if (bytes <= 0)
            break; // If server disconnects or an error occurs, break the loop

        // Lock mutex before printing to std::cout
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Message: " << buffer << std::endl;
        }

        //reprinting prompt after receiving message
        std::cout << "Enter from your side: ";
        std::cout.flush(); // Ensure the prompt is printed immediately
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
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Connected to the server.\n";
    }
    

    // Start a thread to receive messages from the server
    std::thread receiveThread(receiveMessages, clientSocket); 

    // main loop
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Sleep for a short time to prevent busy waiting
        std::string message;
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Enter from your side: "<< std::flush <<std::endl; // Print the prompt without a newline and flush the output
            
        }
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