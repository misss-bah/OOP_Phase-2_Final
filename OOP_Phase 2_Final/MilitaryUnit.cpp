#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
MilitaryUnit::MilitaryUnit(const string& name, int count, int attack, int defense, int speed, int foodConsumption)
    : CombatUnit(name, "", attack, defense, speed), count(count), maxCount(count * 2),
    morale(70), experience(0), foodConsumption(foodConsumption), trainingLevel(50), isVeteran(false) {
}

// Destructor
MilitaryUnit::~MilitaryUnit() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
int MilitaryUnit::getCount() const {
    return count;
}

void MilitaryUnit::setCount(int newCount) {
    if (newCount < 0) newCount = 0;
    if (newCount > maxCount) maxCount = newCount * 2;
    count = newCount;
}

int MilitaryUnit::getMaxCount() const {
    return maxCount;
}

void MilitaryUnit::setMaxCount(int newMax) {
    if (newMax < count) {
        count = newMax;
    }
    maxCount = newMax;
}

int MilitaryUnit::getMorale() const {
    return morale;
}

void MilitaryUnit::adjustMorale(int change) {
    morale += change;

    // Keep morale within bounds
    if (morale < 10) morale = 10;
    if (morale > 100) morale = 100;
}

int MilitaryUnit::getExperience() const {
    return experience;
}

void MilitaryUnit::gainExperience(int amount) {
    experience += amount;

    // Check for veteran status
    if (experience >= 100 && !isVeteran) {
        isVeteran = true;

        // Veterans get bonuses
        attack += 5;
        defense += 5;

        // Adjust description
        description = "Veteran " + name;
    }
}

int MilitaryUnit::getFoodConsumption() const {
    return foodConsumption * count;
}

int MilitaryUnit::getTrainingLevel() const {
    return trainingLevel;
}

bool MilitaryUnit::getIsVeteran() const {
    return isVeteran;
}

// Combat methods
int MilitaryUnit::getCombatStrength() const {
    // Base strength is attack + defense
    int strength = (attack + defense) * count;

    // Morale affects strength
    strength = (strength * morale) / 100;

    // Veterans are more effective
    if (isVeteran) {
        strength = (strength * 120) / 100; // 20% bonus
    }

    return strength;
}

void MilitaryUnit::takeCasualties(int casualties) {
    if (casualties <= 0) return;

    if (casualties >= count) {
        count = 0;
    }
    else {
        count -= casualties;
    }

    // Heavy casualties hurt morale
    if (casualties > count / 10) {
        adjustMorale(-10);
    }
}

// Train the unit
void MilitaryUnit::train() {
    // Gain some experience
    gainExperience(randomInt(1, 5));

    // Small chance to improve stats
    if (randomInt(1, 100) <= 20) {
        attack += 1;
    }

    if (randomInt(1, 100) <= 20) {
        defense += 1;
    }

    // Improve morale and training level
    adjustMorale(5);

    // Increase training level
    trainingLevel += randomInt(1, 3);
    if (trainingLevel > 100) trainingLevel = 100;
}

// Recruit more troops
void MilitaryUnit::recruit(int newTroops) {
    if (newTroops <= 0) return;

    count += newTroops;

    // Expand max count if needed
    if (count > maxCount) {
        maxCount = count * 2;
    }

    // New recruits slightly lower morale and experience
    adjustMorale(-5);
    experience = max(0, experience - 5);
}

// Override base class's calculatePower method
int MilitaryUnit::calculatePower() const {
    return getCombatStrength();
}

// Update method required by Entity base class
void MilitaryUnit::update() {
    if (!getIsActive()) return;

    // Natural morale recovery
    adjustMorale(1);

    // Veteran units gain experience faster
    if (isVeteran && randomInt(1, 100) <= 10) {
        gainExperience(1);
    }
}

// Save military unit data to file
void MilitaryUnit::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save MilitaryUnit: File not open");
    }

    // Call base class save first
    CombatUnit::save(file);

    // Save MilitaryUnit-specific attributes
    file << count << endl;
    file << maxCount << endl;
    file << morale << endl;
    file << experience << endl;
    file << foodConsumption << endl;
    file << trainingLevel << endl;
    file << isVeteran << endl;
}

// Load military unit data from file
void MilitaryUnit::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load MilitaryUnit: File not open");
    }

    // Call base class load first
    CombatUnit::load(file);

    // Load MilitaryUnit-specific attributes
    file >> count;
    file >> maxCount;
    file >> morale;
    file >> experience;
    file >> foodConsumption;
    file >> trainingLevel;
    file >> isVeteran;

    file.ignore(); // Skip newline
}