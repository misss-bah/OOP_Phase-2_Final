#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Army::Army(const string& name)
    : Entity(name, "Kingdom military force"), totalStrength(0), overallMorale(70),
    discipline(65), trainingLevel(50), foodConsumption(50), maxUnits(20), numUnits(0),
    strategy(BALANCED), lastBattleResult(0) {

    // Initialize units array
    units = new MilitaryUnit * [maxUnits];
    for (int i = 0; i < maxUnits; i++) {
        units[i] = nullptr;
    }

    // Add some basic units
    addUnit(new MilitaryUnit("Infantry", 100, 30, 50, 5, 10));
    addUnit(new MilitaryUnit("Archers", 50, 50, 40, 3, 15));
    addUnit(new MilitaryUnit("Cavalry", 80, 70, 60, 8, 25));
}

// Destructor
Army::~Army() {
    // Clean up units
    for (int i = 0; i < numUnits; i++) {
        delete units[i];
    }
    delete[] units;
}

// Getters and setters
int Army::getTotalStrength() const {
    return totalStrength;
}

int Army::getOverallMorale() const {
    return overallMorale;
}

int Army::getMorale() const {
    return overallMorale;
}

void Army::setMorale(int morale) {
    if (morale < 0) morale = 0;
    if (morale > 100) morale = 100;
    overallMorale = morale;
}

int Army::getDiscipline() const {
    return discipline;
}

void Army::setDiscipline(int newDiscipline) {
    if (newDiscipline < 0) newDiscipline = 0;
    if (newDiscipline > 100) newDiscipline = 100;
    discipline = newDiscipline;
}

int Army::getTrainingLevel() const {
    return trainingLevel;
}

void Army::setTrainingLevel(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    trainingLevel = level;
}

int Army::getFoodConsumption() const {
    return foodConsumption;
}

CombatStrategy Army::getStrategy() const {
    return strategy;
}

void Army::setStrategy(CombatStrategy newStrategy) {
    strategy = newStrategy;
}

int Army::getLastBattleResult() const {
    return lastBattleResult;
}

void Army::setLastBattleResult(int result) {
    lastBattleResult = result;
}

// Unit management
void Army::addUnit(MilitaryUnit* unit) {
    if (!unit) {
        throw invalid_argument("Cannot add null unit");
    }

    // Check if we need to resize the array
    if (numUnits >= maxUnits) {
        // Create a new, larger array
        int newMaxUnits = maxUnits * 2;
        MilitaryUnit** newUnits = new MilitaryUnit * [newMaxUnits];

        // Copy existing units to the new array
        for (int i = 0; i < numUnits; i++) {
            newUnits[i] = units[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numUnits; i < newMaxUnits; i++) {
            newUnits[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] units;
        units = newUnits;
        maxUnits = newMaxUnits;
    }

    // Add the new unit
    units[numUnits++] = unit;

    // Recalculate army stats
    calculateTotalStrength();
    calculateOverallMorale();
    calculateFoodConsumption();
}

void Army::removeUnit(int index) {
    if (index < 0 || index >= numUnits) {
        throw out_of_range("Unit index out of range");
    }

    // Delete the unit object
    delete units[index];

    // Shift remaining elements
    for (int i = index; i < numUnits - 1; i++) {
        units[i] = units[i + 1];
    }

    // Set the last position to nullptr and decrement count
    units[--numUnits] = nullptr;

    // Recalculate army stats
    calculateTotalStrength();
    calculateOverallMorale();
    calculateFoodConsumption();
}

MilitaryUnit* Army::getUnit(int index) const {
    if (index < 0 || index >= numUnits) {
        throw out_of_range("Unit index out of range");
    }
    return units[index];
}

int Army::getNumUnits() const {
    return numUnits;
}

// Combat calculations
int Army::calculateAttackPower() const {
    // Base attack is total strength
    int attackPower = totalStrength;

    // Adjust based on morale
    attackPower = (attackPower * overallMorale) / 100;

    // Adjust based on training level
    attackPower = (attackPower * (50 + trainingLevel / 2)) / 100;

    // Adjust based on strategy
    switch (strategy) {
    case AGGRESSIVE:
        attackPower = (attackPower * 130) / 100; // +30% attack
        break;
    case DEFENSIVE:
        attackPower = (attackPower * 80) / 100; // -20% attack
        break;
    case GUERRILLA:
        attackPower = (attackPower * 110) / 100; // +10% attack
        break;
    case BALANCED:
    default:
        // No adjustment
        break;
    }

    return max(1, attackPower);
}

int Army::calculateDefensePower() const {
    // Base defense is total strength / 2 + morale
    int defensePower = (totalStrength / 2) + overallMorale;

    // Adjust based on training level
    defensePower = (defensePower * (50 + trainingLevel / 2)) / 100;

    // Adjust based on strategy
    switch (strategy) {
    case AGGRESSIVE:
        defensePower = (defensePower * 70) / 100; // -30% defense
        break;
    case DEFENSIVE:
        defensePower = (defensePower * 140) / 100; // +40% defense
        break;
    case GUERRILLA:
        defensePower = (defensePower * 120) / 100; // +20% defense
        break;
    case BALANCED:
    default:
        // No adjustment
        break;
    }

    return max(1, defensePower);
}

int Army::calculateMaintenanceCost() const {
    // Base cost is proportional to number of units
    int baseCost = numUnits * 10;

    // Add cost based on total strength
    baseCost += totalStrength / 5;

    // Higher training level costs more
    baseCost += trainingLevel;

    return baseCost;
}

// Getter for maintenance cost that uses the calculate method
int Army::getMaintenanceCost() const {
    return calculateMaintenanceCost();
}

// Apply effects from weather on army
void Army::applyWeatherEffects(const Weather& weather) {
    // Weather severity affects morale
    int moraleChange = -weather.getSeverity();

    // Different weather types have different effects
    switch (weather.getType()) {
    case RAINY:
        moraleChange -= 5;
        break;
    case STORMY:
        moraleChange -= 10;
        break;
    case SNOWY:
        moraleChange -= 15;
        break;
    case DROUGHT:
        foodConsumption = foodConsumption * 120 / 100; // 20% more food consumption
        break;
    case FOGGY:
        // No special effect
        break;
    case SUNNY:
    default:
        moraleChange = 5; // Sunny weather boosts morale
        break;
    }

    // Apply morale change
    adjustMorale(moraleChange);

    // Extreme weather causes casualties
    if (weather.getIsExtreme()) {
        int casualties = totalStrength / 20; // 5% casualties

        // Apply casualties to each unit
        for (int i = 0; i < numUnits; i++) {
            if (units[i]) {
                int unitCasualties = units[i]->getCount() / 20;
                if (unitCasualties < 1) unitCasualties = 1;

                units[i]->takeCasualties(unitCasualties);
            }
        }

        // Recalculate stats
        calculateTotalStrength();
    }
}

// After battle calculation
void Army::processBattleResults(bool victory, int casualtyPercentage) {
    // Apply casualties
    int totalCasualties = (totalStrength * casualtyPercentage) / 100;
    int remainingCasualties = totalCasualties;

    // Distribute casualties among units
    for (int i = 0; i < numUnits && remainingCasualties > 0; i++) {
        if (units[i]) {
            int unitCasualties = (units[i]->getCount() * casualtyPercentage) / 100;
            if (unitCasualties < 1 && units[i]->getCount() > 0) unitCasualties = 1;

            units[i]->takeCasualties(unitCasualties);
            remainingCasualties -= unitCasualties;
        }
    }

    // Adjust morale based on battle outcome
    if (victory) {
        adjustMorale(10);
        lastBattleResult = 1;
    }
    else {
        adjustMorale(-15);
        lastBattleResult = -1;
    }

    // Recalculate army stats
    calculateTotalStrength();
    calculateOverallMorale();
}

// Adjust overall army morale
void Army::adjustMorale(int change) {
    overallMorale += change;

    // Keep morale within bounds
    if (overallMorale < 10) overallMorale = 10;
    if (overallMorale > 100) overallMorale = 100;

    // Propagate morale changes to individual units
    for (int i = 0; i < numUnits; i++) {
        if (units[i]) {
            units[i]->adjustMorale(change / 2); // Half effect on individual units
        }
    }
}

// Training effects
void Army::train() {
    // Increase training level
    trainingLevel += randomInt(1, 5);
    if (trainingLevel > 100) trainingLevel = 100;

    // Improve unit skills
    for (int i = 0; i < numUnits; i++) {
        if (units[i]) {
            units[i]->train();
        }
    }

    // Boost morale
    adjustMorale(5);

    // Recalculate stats
    calculateTotalStrength();
}

// Recruit new troops
void Army::recruit(MilitaryUnit* newUnit) {
    if (!newUnit) {
        throw invalid_argument("Cannot recruit null unit");
    }

    // Add the new unit
    addUnit(newUnit);
}

// Calculate army stats
void Army::calculateTotalStrength() {
    int strength = 0;

    for (int i = 0; i < numUnits; i++) {
        if (units[i]) {
            strength += units[i]->getCombatStrength();
        }
    }

    totalStrength = strength;
}

void Army::calculateOverallMorale() {
    if (numUnits == 0) {
        overallMorale = 50;
        return;
    }

    int totalMorale = 0;

    for (int i = 0; i < numUnits; i++) {
        if (units[i]) {
            totalMorale += units[i]->getMorale();
        }
    }

    overallMorale = totalMorale / numUnits;
}

void Army::calculateFoodConsumption() {
    int consumption = 0;

    for (int i = 0; i < numUnits; i++) {
        if (units[i]) {
            consumption += units[i]->getFoodConsumption();
        }
    }

    foodConsumption = consumption;
}

// Update method required by Entity base class
void Army::update() {
    if (!getIsActive()) return;

    // Update all units
    for (int i = 0; i < numUnits; i++) {
        if (units[i]) {
            units[i]->update();
        }
    }

    // Natural morale recovery
    adjustMorale(1);

    // Recalculate army stats
    calculateTotalStrength();
    calculateOverallMorale();
    calculateFoodConsumption();
}

// Save army data to file
void Army::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save Army: File not open");
    }

    // Call base class save first
    Entity::save(file);

    // Save Army-specific attributes
    file << totalStrength << endl;
    file << overallMorale << endl;
    file << discipline << endl;
    file << trainingLevel << endl;
    file << foodConsumption << endl;
    file << static_cast<int>(strategy) << endl;
    file << lastBattleResult << endl;

    // Save number of units
    file << numUnits << endl;

    // Save each unit
    for (int i = 0; i < numUnits; i++) {
        units[i]->save(file);
    }
}

// Load army data from file
void Army::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load Army: File not open");
    }

    // Call base class load first
    Entity::load(file);

    // Load Army-specific attributes
    file >> totalStrength;
    file >> overallMorale;
    file >> discipline;
    file >> trainingLevel;
    file >> foodConsumption;

    int strategyInt;
    file >> strategyInt;
    strategy = static_cast<CombatStrategy>(strategyInt);

    file >> lastBattleResult;

    // Clean up existing units
    for (int i = 0; i < numUnits; i++) {
        delete units[i];
        units[i] = nullptr;
    }
    numUnits = 0;

    // Load number of units
    int loadNumUnits;
    file >> loadNumUnits;
    file.ignore(); // Skip newline

    // Load each unit
    for (int i = 0; i < loadNumUnits; i++) {
        MilitaryUnit* unit = new MilitaryUnit("Temp", 0, 0, 0, 0, 0);
        unit->load(file);
        addUnit(unit);
    }
}