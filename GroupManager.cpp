#include "GroupManager.h"
#include <iostream>

bool GroupManager::createGroup(const std::string& groupName, const std::string& ownerId) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(groupName) != groups.end()) {
        std::cerr << "Group already exists." << std::endl;
        return false; // Group already exists
    }
    groups[groupName] = Group(ownerId);
    groups[groupName].users.insert(ownerId); // Add owner to the group
    return true;
}

bool GroupManager::joinGroup(const std::string& groupName, const std::string& userId) {
    std::lock_guard<std::mutex> lock(group_mutex);
    auto it = groups.find(groupName);
    if (it == groups.end()) {
        std::cerr << "Group does not exist." << std::endl;
        return false; // Group does not exist
    }
    it->second.users.insert(userId);
    return true;
}

bool GroupManager::leaveGroup(const std::string& groupName, const std::string& userId) {
    std::lock_guard<std::mutex> lock(group_mutex);
    auto it = groups.find(groupName);
    if (it == groups.end()) {
        std::cerr << "Group does not exist." << std::endl;
        return false; // Group does not exist
    }
    it->second.users.erase(userId);

    // If the group is empty, delete it
    if (it->second.users.empty()) {
        groups.erase(groupName);
    }
    return true;
}

bool GroupManager::deleteGroup(const std::string& groupName, const std::string& ownerId) {
    std::lock_guard<std::mutex> lock(group_mutex);
    auto it = groups.find(groupName);
    if (it == groups.end() || it->second.ownerId != ownerId) {
        std::cerr << "Cannot delete group. Either it doesn't exist or you are not the owner." << std::endl;
        return false; // Group does not exist or the user is not the owner
    }
    groups.erase(groupName);
    return true;
}

std::vector<std::string> GroupManager::listGroups() const {
    std::lock_guard<std::mutex> lock(group_mutex);
    std::vector<std::string> groupNames;
    for (const auto& group : groups) {
        groupNames.push_back(group.first);
    }
    return groupNames;
}

std::vector<std::string> GroupManager::listGroupUsers(const std::string& groupName) const {
    std::lock_guard<std::mutex> lock(group_mutex);
    auto it = groups.find(groupName);
    if (it == groups.end()) {
        std::cerr << "Group does not exist." << std::endl;
        return {}; // Group does not exist
    }
    return std::vector<std::string>(it->second.users.begin(), it->second.users.end());
}
