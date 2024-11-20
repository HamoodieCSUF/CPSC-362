#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link Winsock library

using namespace std;

vector<SOCKET> clients; // List of connected client sockets
mutex client_mutex;     // Mutex for thread safety

// Broadcast message to all connected clients
void broadcast_message(const string& message, SOCKET sender) {
    lock_guard<mutex> guard(client_mutex);
    for (SOCKET client : clients) {
        if (client != sender) {
            send(client, message.c_str(), message.size(), 0);
        }
    }
}

// Handle individual client
void handle_client(SOCKET client_socket) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            lock_guard<mutex> guard(client_mutex);
            clients.erase(remove(clients.begin(), clients.end(), client_socket), clients.end());
            closesocket(client_socket);
            cout << "Client disconnected: " << client_socket << endl;
            return;
        }

        buffer[bytes_received] = '\0'; // Null-terminate the message
        string message = string(buffer);
        cout << "Message received: " << message << endl;

        // Broadcast the message to other clients
        broadcast_message(message, client_socket);
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Winsock initialization failed." << endl;
        return -1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        cerr << "Failed to create socket." << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        cerr << "Binding failed." << endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        cerr << "Failed to listen." << endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    cout << "Server is running on port 8080..." << endl;

    while (true) {
        sockaddr_in client_address{};
        int client_len = sizeof(client_address);
        SOCKET client_socket = accept(server_socket, (sockaddr*)&client_address, &client_len);
        if (client_socket == INVALID_SOCKET) {
            cerr << "Failed to accept client." << endl;
            continue;
        }

        cout << "New client connected: " << client_socket << endl;
        {
            lock_guard<mutex> guard(client_mutex);
            clients.push_back(client_socket);
        }

        // Start a new thread to handle the client
        thread(handle_client, client_socket).detach();
    }

    // Cleanup Winsock
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
