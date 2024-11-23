#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

class GroupManager {
private:
    std::unordered_map<std::string, std::unordered_set<std::string>> groups; // GroupName -> Members
    std::unordered_map<std::string, std::string> groupOwners;               // GroupName -> Owner
    std::mutex group_mutex;                                                // Mutex for thread safety

public:
    bool createGroup(const std::string& groupName, const std::string& owner);
    bool joinGroup(const std::string& groupName, const std::string& username);
    bool leaveGroup(const std::string& groupName, const std::string& username);
    std::vector<std::string> listGroups() const;
    std::unordered_set<std::string> listGroupMembers(const std::string& groupName) const;
    bool deleteGroup(const std::string& groupName, const std::string& owner);
};

#endif // GROUP_MANAGER_H
