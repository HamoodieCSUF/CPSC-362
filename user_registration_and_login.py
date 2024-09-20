
# Dictionary to store user information
user_database = {}

# Function for user registration
def register_user(username, password):
    if username in user_database:
        print("Username already exists. Please try a different one.")
    else:
        user_database[username] = password
        print(f"User '{username}' registered successfully!")

# Function for user login
def login_user(username, password):
    if username in user_database:
        if user_database[username] == password:
            print(f"Login successful! Welcome, {username}.")
        else:
            print("Incorrect password. Please try again.")
    else:
        print("Username not found. Please register first.")

# Main execution for testing
while True:
    print("\n1. Register")
    print("2. Login")
    print("3. Exit")
    choice = input("Choose an option: ")

    if choice == '1':
        username = input("Enter username to register: ")
        password = input("Enter password: ")
        register_user(username, password)
    elif choice == '2':
        username = input("Enter username to login: ")
        password = input("Enter password: ")
        login_user(username, password)
    elif choice == '3':
        print("Exiting program.")
        break
    else:
        print("Invalid option. Please choose again.")
