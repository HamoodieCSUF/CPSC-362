#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

// Dictionary to store user information
unordered_map<string, string> user_database;

// Function for user registration
void register_user(const string& username, const string& password) {
    if (user_database.find(username) != user_database.end()) {
        cout << "Username already exists. Please try a different one." << endl;
    } else {
        user_database[username] = password;
        cout << "User '" << username << "' registered successfully!" << endl;
    }
}

// Function for user login
void login_user(const string& username, const string& password) {
    if (user_database.find(username) != user_database.end()) {
        if (user_database[username] == password) {
            cout << "Login successful! Welcome, " << username << "." << endl;
        } else {
            cout << "Incorrect password. Please try again." << endl;
        }
    } else {
        cout << "Username not found. Please register first." << endl;
    }
}

// Main execution for testing
int main() {
    while (true) {
        cout << "\n1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "3. Exit" << endl;
        cout << "Choose an option: ";

        int choice;
        cin >> choice;
        cin.ignore(); // Ignore the newline character left in the input buffer

        if (choice == 1) {
            string username, password;
            cout << "Enter username to register: ";
            getline(cin, username);
            cout << "Enter password: ";
            getline(cin, password);
            register_user(username, password);
        } else if (choice == 2) {
            string username, password;
            cout << "Enter username to login: ";
            getline(cin, username);
            cout << "Enter password: ";
            getline(cin, password);
            login_user(username, password);
        } else if (choice == 3) {
            cout << "Exiting program." << endl;
            break;
        } else {
            cout << "Invalid option. Please choose again." << endl;
        }
    }

    return 0;
}
