#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
CombatUnit::CombatUnit(const string& name, const string& description, int attack, int defense, int speed)
    : Entity(name, description), attack(attack), defense(defense), speed(speed) {
}

// Destructor
CombatUnit::~CombatUnit() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
int CombatUnit::getAttack() const {
    return attack;
}

void CombatUnit::setAttack(int value) {
    if (value < 0) value = 0;
    attack = value;
}

int CombatUnit::getDefense() const {
    return defense;
}

void CombatUnit::setDefense(int value) {
    if (value < 0) value = 0;
    defense = value;
}

int CombatUnit::getSpeed() const {
    return speed;
}

void CombatUnit::setSpeed(int value) {
    if (value < 0) value = 0;
    speed = value;
}

// Combat methods
int CombatUnit::calculatePower() const {
    return (attack + defense) * 2 + speed;
}

void CombatUnit::train() {
    // Increase combat stats
    int attackBoost = randomInt(1, 3);
    int defenseBoost = randomInt(1, 2);
    int speedBoost = randomInt(0, 1);

    attack += attackBoost;
    defense += defenseBoost;
    speed += speedBoost;
}

void CombatUnit::takeDamage(int damage) {
    // Base implementation does nothing specific
    // This will be overridden by derived classes
}

int CombatUnit::dealDamage(CombatStrategy strategy) {
    int baseDamage = attack;

    // Apply strategy modifiers
    switch (strategy) {
    case AGGRESSIVE:
        baseDamage = baseDamage * 130 / 100; // 30% more damage
        break;
    case DEFENSIVE:
        baseDamage = baseDamage * 70 / 100; // 30% less damage but better defense
        break;
    case GUERRILLA:
        baseDamage = (randomInt(0, 100) < 30) ? baseDamage * 2 : baseDamage / 2; // 30% chance of double damage, otherwise half
        break;
    case BALANCED:
    default:
        // No modifier for balanced
        break;
    }

    return baseDamage;
}

void CombatUnit::applyWeatherEffects(const Weather& weather) {
    // Different weather effects on combat units
    switch (weather.getType()) {
    case STORMY:
    case RAINY:
        speed = max(1, speed - 1);
        break;
    case SNOWY:
        speed = max(1, speed - 2);
        attack = max(1, attack - 1);
        break;
    case SUNNY:
        attack += 1; // Better visibility
        break;
    default:
        break;
    }
}

void CombatUnit::applyLeaderEffects(const Leader& leader) {
    // Apply leader's combat bonuses based on military skill
    if (leader.getMilitarySkill() > 70) {
        attack += 2;
        defense += 1;
    }
    else if (leader.getMilitarySkill() > 50) {
        attack += 1;
    }

    // A smart leader improves all unit performance
    if (leader.getIntelligence() > 70) {
        speed += 1;
    }
}

// Update method required by Entity base class
void CombatUnit::update() {
    // This is implemented by child classes
}

// Save combat unit data to file
void CombatUnit::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save CombatUnit: File not open");
    }

    // Call base class save first
    Entity::save(file);

    // Save CombatUnit-specific attributes
    file << attack << endl;
    file << defense << endl;
    file << speed << endl;
}

// Load combat unit data from file
void CombatUnit::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load CombatUnit: File not open");
    }

    // Call base class load first
    Entity::load(file);

    // Load CombatUnit-specific attributes
    file >> attack;
    file >> defense;
    file >> speed;

    file.ignore(); // Skip newline
}