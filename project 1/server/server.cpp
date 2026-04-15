#include <iostream>
#include <winsock2.h>   // For socket functions
#include <ws2tcpip.h>   // For sockaddr_in structure and inet_pton function
#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

int main(){

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock

    //creat a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    //bind the socket to an address and port
    sockaddr_in server_address;
    //!server can produce garbacge values if not initialized properly. To prevent this, we use memset to set all bytes of the server_address structure to zero before assigning values to its fields. This ensures that any unused fields are initialized to zero, which can help avoid unintended behavior when the structure is used in socket operations.
    memset(&server_address, 0, sizeof(server_address)); // Zero out the server_address structure
    server_address.sin_family = AF_INET;        //! IPv4
    server_address.sin_addr.s_addr = INADDR_ANY; //! Bind to all interfaces 
    server_address.sin_port = htons(8081);  //! Port 8081 
    
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Failed to bind socket, the errror is :"<< WSAGetLastError() << std::endl;
        closesocket(server_socket);
        return 1;
    }


    // listen for incoming connections
    listen(server_socket, 5); // Listen with a backlog of 5
    std::cout << "Server is listening on port 8081..." << std::endl;  
    
    if(listen(server_socket, 5) == -1) {
        std::cerr << "Failed to listen on socket" << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        return 1;
    }


    // accept a connection
    int address_length = sizeof(server_address);//? address_length is an integer variable that holds the size of the server_address structure. It is used as an argument in the accept function to specify the size of the address structure being passed. The accept function will update this variable with the actual size of the client's address information when a connection is accepted.
    int client_socket = accept(server_socket, (struct sockaddr*)&server_address, &address_length);
    
    if (client_socket == -1) {
        std::cerr << "Failed to accept connection" << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        return 1;
    }

    std::cout << "Client connected!" << std::endl;


    while(true) {
        // Read data from the client
        char buffer[1024] = {0}; // Buffer to hold incoming data
        recv(client_socket, buffer, sizeof(buffer), 0); // Read data from the client into the buffer
        std::cout << "Received data: " << buffer << std::endl;
    
        // Send a response back to the client
        std::string response;
        std::cout <<"you can type a response to the client: "<<std::endl;
        std::getline(std::cin, response); // Read a line of input from the console

        if(response == "exit") {
            break; // Exit the loop if the user types "exit"
        }
        send(client_socket, response.c_str(), response.length(), 0); // Send the response back to the client

        //!resonse.c_str() is a function call that returns a pointer to a null-terminated character array (C-style string) representation of the std::string object response. This is necessary because the send function expects a C-style string as its argument for the data to be sent. By calling response.c_str(), we can pass the contents of the std::string as a C-style string to the send function, allowing us to send the response back to the client over the socket connection.
        //!const char* ptr = response.c_str();
    }

       


    // Close the sockets
    closesocket(client_socket);//? close(client_socket) is a function call that closes the client socket. This is important to free up system resources and allow other clients to connect to the server. After this call, the client socket will no longer be valid and cannot be used for communication.
    closesocket(server_socket);//? close(server_socket) is a function call that closes the server socket. This is important to free up system resources and allow the server to shut down gracefully. After this call, the server socket will no longer be valid and cannot be used for accepting new connections.
    WSACleanup(); // Clean up Winsock resources

    std::cout<< "Server is shutting down." << std::endl;
    
    return 0;
}