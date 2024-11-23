#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "GroupManager.h" // Include group management
#include "user_registration_and_login.cpp" // Include user registration and login

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

using namespace std;

vector<SOCKET> clients;  // List of connected client sockets
mutex client_mutex;      // Mutex for thread safety
GroupManager groupManager; // Instantiate GroupManager

// Broadcast message to all connected clients
void broadcast_message(const string& message, SOCKET sender) {
    lock_guard<mutex> guard(client_mutex);
    for (SOCKET client : clients) {
        if (client != sender) {
            send(client, message.c_str(), message.size(), 0);
        }
    }
}

// Authenticate the client (registration or login)
bool authenticate_client(SOCKET client_socket, string& username) {
    char buffer[1024];
    send(client_socket, "1. Register\n2. Login\nChoose an option: ", 40, 0);

    recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    int option = atoi(buffer);

    send(client_socket, "Enter username: ", 17, 0);
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    username = string(buffer).substr(0, string(buffer).find('\n'));

    send(client_socket, "Enter password: ", 17, 0);
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    string password = string(buffer).substr(0, string(buffer).find('\n'));

    if (option == 1) {
        register_user(username, password);
        send(client_socket, "Registration successful! Please log in.\n", 40, 0);
        return authenticate_client(client_socket, username);
    } else if (option == 2) {
        if (user_database.find(username) != user_database.end() && user_database[username] == password) {
            send(client_socket, "Login successful!\n", 19, 0);
            return true;
        } else {
            send(client_socket, "Login failed. Disconnecting.\n", 29, 0);
            return false;
        }
    } else {
        send(client_socket, "Invalid option. Disconnecting.\n", 31, 0);
        return false;
    }
}

// Handle group commands
void handle_group_commands(SOCKET client_socket, const string& username) {
    char buffer[1024];
    while (true) {
        send(client_socket, "Group Commands:\n1. Create Group\n2. Join Group\n3. Leave Group\n4. List Groups\n5. Delete Group\n6. Back to Chat\nChoose an option: ", 120, 0);
        recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        int option = atoi(buffer);

        if (option == 1) {
            send(client_socket, "Enter group name: ", 19, 0);
            recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            string groupName(buffer);
            if (groupManager.createGroup(groupName, username)) {
                send(client_socket, "Group created successfully.\n", 28, 0);
            } else {
                send(client_socket, "Group creation failed (already exists).\n", 41, 0);
            }
        } else if (option == 2) {
            send(client_socket, "Enter group name: ", 19, 0);
            recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            string groupName(buffer);
            if (groupManager.joinGroup(groupName, username)) {
                send(client_socket, "Joined group successfully.\n", 27, 0);
            } else {
                send(client_socket, "Group does not exist.\n", 23, 0);
            }
        } else if (option == 3) {
            send(client_socket, "Enter group name: ", 19, 0);
            recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            string groupName(buffer);
            if (groupManager.leaveGroup(groupName, username)) {
                send(client_socket, "Left group successfully.\n", 26, 0);
            } else {
                send(client_socket, "Group does not exist or you are not a member.\n", 45, 0);
            }
        } else if (option == 4) {
            vector<string> groups = groupManager.listGroups();
            string response = "Available Groups:\n";
            for (const string& group : groups) {
                response += group + "\n";
            }
            send(client_socket, response.c_str(), response.size(), 0);
        } else if (option == 5) {
            send(client_socket, "Enter group name: ", 19, 0);
            recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            string groupName(buffer);
            if (groupManager.deleteGroup(groupName, username)) {
                send(client_socket, "Group deleted successfully.\n", 29, 0);
            } else {
                send(client_socket, "Failed to delete group (not owner or does not exist).\n", 53, 0);
            }
        } else if (option == 6) {
            break; // Exit group commands
        } else {
            send(client_socket, "Invalid option.\n", 17, 0);
        }
    }
}

// Handle individual client
void handle_client(SOCKET client_socket) {
    string username;
    if (!authenticate_client(client_socket, username)) {
        closesocket(client_socket);
        return;
    }

    {
        lock_guard<mutex> guard(client_mutex);
        clients.push_back(client_socket);
    }

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

        if (message == "/groups") {
            handle_group_commands(client_socket, username);
        } else {
            cout << "Message received: " << message << endl;
            broadcast_message(message, client_socket);
        }
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

        // Start a new thread to handle the client
        thread(handle_client, client_socket).detach();
    }

    // Cleanup Winsock
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
