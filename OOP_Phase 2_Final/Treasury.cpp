#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Treasury::Treasury(const string& name)
    : Entity(name, "Kingdom treasury system"), gold(0), income(0), expenses(0),
    taxIncome(0), tradeIncome(0), otherIncome(0), militaryExpenses(0),
    buildingExpenses(0), otherExpenses(0), corruption(0), inflation(0) {
}

// Destructor
Treasury::~Treasury() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
int Treasury::getGold() const {
    return gold;
}

void Treasury::setGold(int amount) {
    gold = max(0, amount);
}

int Treasury::getIncome() const {
    return income;
}

void Treasury::setIncome(int amount) {
    income = max(0, amount);
}

int Treasury::getExpenses() const {
    return expenses;
}

void Treasury::setExpenses(int amount) {
    expenses = max(0, amount);
}

int Treasury::getTaxIncome() const {
    return taxIncome;
}

void Treasury::setTaxIncome(int amount) {
    taxIncome = max(0, amount);
    recalculateTotalIncome();
}

int Treasury::getTradeIncome() const {
    return tradeIncome;
}

void Treasury::setTradeIncome(int amount) {
    tradeIncome = max(0, amount);
    recalculateTotalIncome();
}

int Treasury::getOtherIncome() const {
    return otherIncome;
}

void Treasury::setOtherIncome(int amount) {
    otherIncome = max(0, amount);
    recalculateTotalIncome();
}

int Treasury::getMilitaryExpenses() const {
    return militaryExpenses;
}

void Treasury::setMilitaryExpenses(int amount) {
    militaryExpenses = max(0, amount);
    recalculateExpenses();
}

int Treasury::getBuildingExpenses() const {
    return buildingExpenses;
}

void Treasury::setBuildingExpenses(int amount) {
    buildingExpenses = max(0, amount);
    recalculateExpenses();
}

int Treasury::getOtherExpenses() const {
    return otherExpenses;
}

void Treasury::setOtherExpenses(int amount) {
    otherExpenses = max(0, amount);
    recalculateExpenses();
}

int Treasury::getCorruption() const {
    return corruption;
}

void Treasury::setCorruption(int level) {
    corruption = max(0, min(100, level));
}

double Treasury::getInflation() const {
    return inflation;
}

void Treasury::setInflation(double rate) {
    inflation = max(0.0, min(50.0, rate));
}

// Recalculate total income
void Treasury::recalculateTotalIncome() {
    income = taxIncome + tradeIncome + otherIncome;
}

// Recalculate total expenses
void Treasury::recalculateExpenses() {
    expenses = militaryExpenses + buildingExpenses + otherExpenses;
}

// Calculate balance (income - expenses)
int Treasury::calculateBalance() const {
    return income - expenses;
}

// Treasury operations
bool Treasury::spend(int amount) {
    if (amount <= 0) {
        return true; // Nothing to spend
    }

    if (amount > gold) {
        return false; // Not enough gold
    }

    gold -= amount;
    return true;
}

void Treasury::earn(int amount) {
    if (amount <= 0) {
        return; // Nothing to earn
    }

    gold += amount;
}

void Treasury::deposit(int amount) {
    if (amount <= 0) {
        return; // Nothing to deposit
    }

    gold += amount;
}

// Calculate tax income based on population and tax rates
void Treasury::calculateTaxIncome(const Population* population) {
    if (!population) {
        return;
    }

    int totalTax = 0;

    // Calculate tax income from each social class
    for (int i = 0; i < population->getNumClasses(); i++) {
        SocialClass* socialClass = population->getSocialClass(i);
        if (socialClass) {
            totalTax += socialClass->calculateTaxRevenue();
        }
    }

    // Apply corruption (reduces tax income)
    int corruptionLoss = (totalTax * corruption) / 100;
    totalTax -= corruptionLoss;

    // Set the new tax income
    setTaxIncome(max(0, totalTax));
}

// Calculate trade income based on economy's trade level
void Treasury::calculateTradeIncome(int tradeLevel, int numResources) {
    // Base trade income
    int baseTrade = tradeLevel * 2;

    // Adjust based on number of resources (more resources = more trade potential)
    int resourceFactor = numResources * 5;

    // Calculate total trade income
    int totalTrade = baseTrade + resourceFactor;

    // Apply inflation effect (reduces real income)
    totalTrade = totalTrade * (100 - (int)inflation) / 100;

    // Set the new trade income
    setTradeIncome(max(0, totalTrade));
}

// Calculate military expenses based on army
void Treasury::calculateMilitaryExpenses(const Army* army) {
    if (!army) {
        setMilitaryExpenses(0);
        return;
    }

    // Base expense is the army's maintenance cost
    int expense = army->calculateMaintenanceCost();

    // Apply inflation (increases expenses)
    expense = expense * (100 + (int)inflation) / 100;

    // Set the new military expenses
    setMilitaryExpenses(max(0, expense));
}

// Apply turn-based treasury changes
void Treasury::applyTurnChanges() {
    // Calculate net income/expense
    int netChange = calculateBalance();

    // Apply to treasury
    gold += netChange;

    // Ensure gold doesn't go negative
    if (gold < 0) {
        gold = 0;
        // In a more complex system, this could trigger a financial crisis event
    }
}

// Update method required by Entity base class
void Treasury::update() {
    if (!isActive) return;

    // Apply turn changes
    applyTurnChanges();

    // Other updates could be added here
}

// Save treasury data to file
void Treasury::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save Treasury: File not open");
    }

    // Call base class save first
    Entity::save(file);

    // Save Treasury-specific attributes
    file << gold << endl;
    file << income << endl;
    file << expenses << endl;
    file << taxIncome << endl;
    file << tradeIncome << endl;
    file << otherIncome << endl;
    file << militaryExpenses << endl;
    file << buildingExpenses << endl;
    file << otherExpenses << endl;
    file << corruption << endl;
    file << inflation << endl;
}

// Load treasury data from file
void Treasury::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load Treasury: File not open");
    }

    // Call base class load first
    Entity::load(file);

    // Load Treasury-specific attributes
    file >> gold;
    file >> income;
    file >> expenses;
    file >> taxIncome;
    file >> tradeIncome;
    file >> otherIncome;
    file >> militaryExpenses;
    file >> buildingExpenses;
    file >> otherExpenses;
    file >> corruption;
    file >> inflation;

    file.ignore(); // Skip newline
}