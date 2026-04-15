#include<iostream>
#include <winsock2.h>   // For socket functions
#include <ws2tcpip.h>   // For sockaddr_in structure and inet_pton function
#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock

    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Failed to create socket" << WSAGetLastError() << std::endl;
        return 1;
    }

    // Define the server address
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); // Zero out the server_address structure
    server_address.sin_family = AF_INET; // IPv4
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr); // Server IP address 
    server_address.sin_port = htons(8081); // Server port


    // Connect to the server

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Failed to connect to server" << WSAGetLastError() << std::endl;
        closesocket(client_socket);
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    
    while(true) {
        // Send a message to the server
        std::string message ;
        std::cout << "Enter a message to send to the server (type 'exit' to quit): ";
        std::getline(std::cin, message); // Read a line of input from the user
        if (message == "exit") {
            break;
        }
        send(client_socket, message.c_str(), message.length(), 0); // Send the message to the server

            // Read a response from the server
        char buffer[1024] = {0}; // Buffer to hold incoming data
        recv(client_socket, buffer, sizeof(buffer), 0); // Read data from the server

        if(recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
            std::cerr << "Failed to receive data from server" << WSAGetLastError() << std::endl;
            closesocket(client_socket);
            return 1;
        }
        std::cout << "Message from server: " << buffer << std::endl; // Print the response from the server
    }


    // Close the connection
    closesocket(client_socket); // Close the client socket
    std::cout << "Closing connection..." << std::endl;
    
    
    return 0;
}   
