#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Resource::Resource(const string& name, ResourceType type, int amount, int gatherRate, int consumptionRate)
    : Entity(name), type(type), amount(amount), gatherRate(gatherRate), consumptionRate(consumptionRate),
    value(10), isStockpiled(false) {
}

Resource::~Resource() {
    // Cleanup
}

ResourceType Resource::getType() const {
    return type;
}

int Resource::getAmount() const {
    return amount;
}

void Resource::setAmount(int newAmount) {
    if (newAmount < 0) {
        throw invalid_argument("Resource amount cannot be negative");
    }
    amount = newAmount;
}

void Resource::gather() {
    amount += gatherRate;
}

bool Resource::consume(int consumeAmount) {
    if (consumeAmount <= 0) {
        throw invalid_argument("Consumption amount must be positive");
    }

    if (amount >= consumeAmount) {
        amount -= consumeAmount;
        return true;
    }
    return false;
}

void Resource::adjustGatherRate(int rate) {
    if (gatherRate + rate < 0) {
        gatherRate = 0;
    }
    else {
        gatherRate += rate;
    }
}

void Resource::adjustConsumptionRate(int rate) {
    if (consumptionRate + rate < 0) {
        consumptionRate = 0;
    }
    else {
        consumptionRate += rate;
    }
}

int Resource::getGatherRate() const {
    return gatherRate;
}

void Resource::setGatherRate(int rate) {
    if (rate < 0) {
        gatherRate = 0;
    }
    else {
        gatherRate = rate;
    }
}

int Resource::getConsumptionRate() const {
    return consumptionRate;
}

void Resource::setConsumptionRate(int rate) {
    if (rate < 0) {
        consumptionRate = 0;
    }
    else {
        consumptionRate = rate;
    }
}

int Resource::getValue() const {
    return value;
}

void Resource::setValue(int newValue) {
    if (newValue <= 0) {
        value = 1; // Minimum value of 1
    }
    else {
        value = newValue;
    }
}

bool Resource::getIsStockpiled() const {
    return isStockpiled;
}

void Resource::setIsStockpiled(bool stockpiled) {
    isStockpiled = stockpiled;
}

void Resource::update() {
    gather();

    // Natural consumption
    if (consumptionRate > 0) {
        consume(consumptionRate);
    }
}

void Resource::save(ofstream& file) const {
    Entity::save(file);
    file << static_cast<int>(type) << endl;
    file << amount << endl;
    file << gatherRate << endl;
    file << consumptionRate << endl;
}

void Resource::load(ifstream& file) {
    Entity::load(file);
    int typeInt;
    file >> typeInt;
    type = static_cast<ResourceType>(typeInt);
    file >> amount;
    file >> gatherRate;
    file >> consumptionRate;
    file.ignore(); // Skip newline
}
