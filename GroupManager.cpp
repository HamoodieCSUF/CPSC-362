#include "GroupManager.h"
#include <iostream>

bool GroupManager::createGroup(const std::string& groupName, const std::string& owner) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(groupName) != groups.end()) {
        return false; // Group already exists
    }
    groups[groupName] = {};
    groupOwners[groupName] = owner;
    groups[groupName].insert(owner); // Owner is automatically a member
    return true;
}

bool GroupManager::joinGroup(const std::string& groupName, const std::string& username) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(groupName) == groups.end()) {
        return false; // Group does not exist
    }
    groups[groupName].insert(username);
    return true;
}

bool GroupManager::leaveGroup(const std::string& groupName, const std::string& username) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(groupName) == groups.end()) {
        return false; // Group does not exist
    }
    groups[groupName].erase(username);
    if (groups[groupName].empty()) {
        groups.erase(groupName);
        groupOwners.erase(groupName); // Remove group if empty
    }
    return true;
}

std::vector<std::string> GroupManager::listGroups() const {
    std::lock_guard<std::mutex> lock(group_mutex);
    std::vector<std::string> groupList;
    for (const auto& group : groups) {
        groupList.push_back(group.first);
    }
    return groupList;
}

std::unordered_set<std::string> GroupManager::listGroupMembers(const std::string& groupName) const {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(groupName) != groups.end()) {
        return groups.at(groupName);
    }
    return {};
}

bool GroupManager::deleteGroup(const std::string& groupName, const std::string& owner) {
    std::lock_guard<std::mutex> lock(group_mutex);
    if (groups.find(groupName) == groups.end() || groupOwners[groupName] != owner) {
        return false; // Group does not exist or user is not the owner
    }
    groups.erase(groupName);
    groupOwners.erase(groupName);
    return true;
}
