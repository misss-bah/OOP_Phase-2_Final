#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Entity::Entity(const string& name, const string& description)
    : name(name), description(description), isActive(true) {
}

// Destructor
Entity::~Entity() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
string Entity::getName() const {
    return name;
}

void Entity::setName(const string& newName) {
    name = newName;
}

string Entity::getDescription() const {
    return description;
}

void Entity::setDescription(const string& newDescription) {
    description = newDescription;
}

bool Entity::getIsActive() const {
    return isActive;
}

void Entity::setIsActive(bool active) {
    isActive = active;
}

// Save entity data to file
void Entity::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save Entity: File not open");
    }

    file << name << endl;
    file << description << endl;
    file << isActive << endl;
}

// Load entity data from file
void Entity::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load Entity: File not open");
    }

    getline(file, name);
    getline(file, description);
    file >> isActive;
    file.ignore(); // Skip newline
}