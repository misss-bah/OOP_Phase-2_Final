#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor implementation
Human::Human(const string& name, int happiness, int health, int loyaltyLevel)
    : Entity(name), happiness(happiness), health(health), loyaltyLevel(loyaltyLevel) {
    // Validate and constrain initial values
    if (happiness < 0) happiness = 0;
    if (happiness > 100) happiness = 100;
    if (health < 0) health = 0;
    if (health > 100) health = 100;
    if (loyaltyLevel < 0) loyaltyLevel = 0;
    if (loyaltyLevel > 100) loyaltyLevel = 100;
}

// Destructor
Human::~Human() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
int Human::getHappiness() const {
    return happiness;
}

void Human::adjustHappiness(int change) {
    happiness += change;

    // Constrain happiness to valid range
    if (happiness < 0) happiness = 0;
    if (happiness > 100) happiness = 100;
}

int Human::getHealth() const {
    return health;
}

void Human::adjustHealth(int change) {
    health += change;

    // Constrain health to valid range
    if (health < 0) health = 0;
    if (health > 100) health = 100;
}

int Human::getLoyaltyLevel() const {
    return loyaltyLevel;
}

void Human::adjustLoyalty(int change) {
    loyaltyLevel += change;

    // Constrain loyalty to valid range
    if (loyaltyLevel < 0) loyaltyLevel = 0;
    if (loyaltyLevel > 100) loyaltyLevel = 100;
}

// Determine if unrest is likely based on current attributes
bool Human::isUnrestLikely() const {
    // Unrest is likely if happiness or loyalty are very low
    return (happiness < 30 || loyaltyLevel < 25);
}

// Apply effects of disease
void Human::applyDiseaseEffect(const Disease& disease) {
    // Disease reduces health based on severity
    int healthReduction = disease.getSeverity() * 2;
    adjustHealth(-healthReduction);

    // Disease also reduces happiness
    adjustHappiness(-disease.getSeverity());
}

// Apply effects of weather
void Human::applyWeatherEffect(const Weather& weather) {
    // Extreme weather reduces happiness
    if (weather.getIsExtreme()) {
        adjustHappiness(-5);
    }

    // Weather affects morale
    adjustHappiness(weather.getMoraleEffect());
}

// Update method required by Entity base class
void Human::update() {
    // Natural healing if health is below 100 but above 20
    if (health < 100 && health > 20) {
        adjustHealth(1);
    }

    // Natural happiness decline if very high
    if (happiness > 90) {
        adjustHappiness(-1);
    }

    // Natural happiness improvement if very low
    if (happiness < 10) {
        adjustHappiness(1);
    }
}

// Save human data to file
void Human::save(ofstream& file) const {
    // Call base class save first
    Entity::save(file);

    // Save Human-specific attributes
    file << happiness << endl;
    file << health << endl;
    file << loyaltyLevel << endl;
}

// Load human data from file
void Human::load(ifstream& file) {
    // Call base class load first
    Entity::load(file);

    // Load Human-specific attributes
    file >> happiness;
    file >> health;
    file >> loyaltyLevel;
    file.ignore(); // Skip newline
}
