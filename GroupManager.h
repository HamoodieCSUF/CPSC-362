#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

class User {
public:
    std::string id;
    std::string name;

    User(const std::string& userId, const std::string& userName)
        : id(userId), name(userName) {}
};

class Group {
public:
    std::unordered_set<std::string> users; // User IDs
    std::string ownerId;                  // Group owner ID

    Group(const std::string& owner)
        : ownerId(owner) {}
};

class GroupManager {
private:
    std::unordered_map<std::string, Group> groups; // GroupName -> Group
    std::mutex group_mutex;                       // Mutex for thread safety

public:
    bool createGroup(const std::string& groupName, const std::string& ownerId);
    bool joinGroup(const std::string& groupName, const std::string& userId);
    bool leaveGroup(const std::string& groupName, const std::string& userId);
    bool deleteGroup(const std::string& groupName, const std::string& ownerId);
    std::vector<std::string> listGroups() const;
    std::vector<std::string> listGroupUsers(const std::string& groupName) const;
};

#endif // GROUP_MANAGER_H
