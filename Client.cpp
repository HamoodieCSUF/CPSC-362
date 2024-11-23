#include <iostream>
#include <thread>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link Winsock library

using namespace std;

// Function to handle incoming messages from the server
void receive_messages(SOCKET server_socket) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(server_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            cout << "Disconnected from the server." << endl;
            closesocket(server_socket);
            return;
        }

        buffer[bytes_received] = '\0'; // Null-terminate the message
        cout << "Message: " << buffer << endl;
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Winsock initialization failed." << endl;
        return -1;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        cerr << "Failed to create socket." << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        cerr << "Failed to connect to the server." << endl;
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }

    cout << "Connected to the server." << endl;

    // Start a thread to handle receiving messages
    thread(receive_messages, client_socket).detach();

    // Send messages to the server
    while (true) {
        string message;
        cout << "Enter message: ";
        getline(cin, message);

        if (message == "exit") {
            break;
        }

        send(client_socket, message.c_str(), message.size(), 0);
    }

    // Cleanup
    closesocket(client_socket);
    WSACleanup();
    return 0;
}

