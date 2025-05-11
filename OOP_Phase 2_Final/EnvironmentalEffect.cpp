#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
EnvironmentalEffect::EnvironmentalEffect(const string& name, const string& description, int severity, int duration)
    : Entity(name, description), severity(severity), duration(duration), turnsActive(0) {
    // Validate parameters
    if (severity < 1) {
        severity = 1;
    }

    if (duration < 1) {
        duration = 1;
    }
}

// Destructor
EnvironmentalEffect::~EnvironmentalEffect() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
int EnvironmentalEffect::getSeverity() const {
    return severity;
}

void EnvironmentalEffect::setSeverity(int newSeverity) {
    if (newSeverity < 1) {
        throw invalid_argument("Severity must be at least 1");
    }
    severity = newSeverity;
}

int EnvironmentalEffect::getDuration() const {
    return duration;
}

void EnvironmentalEffect::setDuration(int newDuration) {
    if (newDuration < 1) {
        throw invalid_argument("Duration must be at least 1");
    }
    duration = newDuration;
}

int EnvironmentalEffect::getTurnsActive() const {
    return turnsActive;
}

void EnvironmentalEffect::incrementTurnsActive() {
    turnsActive++;
}

int EnvironmentalEffect::getTurnsRemaining() const {
    return max(0, duration - turnsActive);
}

// Save environmental effect data to file
void EnvironmentalEffect::save(ofstream& file) const {
    // Call base class save first
    Entity::save(file);

    // Save EnvironmentalEffect-specific attributes
    file << severity << endl;
    file << duration << endl;
    file << turnsActive << endl;
}

// Load environmental effect data from file
void EnvironmentalEffect::load(ifstream& file) {
    // Call base class load first
    Entity::load(file);

    // Load EnvironmentalEffect-specific attributes
    file >> severity;
    file >> duration;
    file >> turnsActive;
    file.ignore(); // Skip newline
}
