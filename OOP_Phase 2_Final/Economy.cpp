#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Economy::Economy(const string& name, Treasury* treasury)
    : Entity(name, "Kingdom economy system"), treasury(treasury), productionLevel(50), tradeLevel(50),
    taxRate(10.0), tariffRate(5.0), marketStability(50), inflation(0.0), employmentRate(80),
    corruptionLevel(0), maxResources(10), numResources(0), bank(nullptr),
    tradeRoutes(nullptr), numTradeRoutes(0), maxTradeRoutes(5) {

    // Initialize resources array
    resources = new Resource * [maxResources];
    for (int i = 0; i < maxResources; i++) {
        resources[i] = nullptr;
    }

    // Initialize trade routes array
    tradeRoutes = new TradeRoute * [maxTradeRoutes];
    for (int i = 0; i < maxTradeRoutes; i++) {
        tradeRoutes[i] = nullptr;
    }

    // Create default resources
    addResource(new Resource("Food", FOOD, 1000, 100, 80));
    addResource(new Resource("Wood", WOOD, 500, 50, 30));
    addResource(new Resource("Stone", STONE, 300, 30, 20));
    addResource(new Resource("Gold", GOLD, 200, 20, 10));
    addResource(new Resource("Iron", IRON, 100, 10, 5));

    // Set initial treasury values if provided
    if (treasury) {
        treasury->setIncome(100);
        treasury->setExpenses(80);
    }
}

// Destructor
Economy::~Economy() {
    // Clean up resources
    for (int i = 0; i < numResources; i++) {
        delete resources[i];
    }
    delete[] resources;

    // Clean up trade routes
    for (int i = 0; i < numTradeRoutes; i++) {
        delete tradeRoutes[i];
    }
    delete[] tradeRoutes;

    // treasury is not owned by Economy, so don't delete it here
}

// Getters and setters
int Economy::getProductionLevel() const {
    return productionLevel;
}

void Economy::setProductionLevel(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    productionLevel = level;
}

int Economy::getTradeLevel() const {
    return tradeLevel;
}

void Economy::setTradeLevel(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    tradeLevel = level;
}

double Economy::getTaxRate() const {
    return taxRate;
}

void Economy::setTaxRate(double rate) {
    if (rate < 0.0) rate = 0.0;
    if (rate > 100.0) rate = 100.0;
    taxRate = rate;
}

int Economy::getMarketStability() const {
    return marketStability;
}

void Economy::setMarketStability(int stability) {
    if (stability < 0) stability = 0;
    if (stability > 100) stability = 100;
    marketStability = stability;
}

double Economy::getInflation() const {
    return inflation;
}

void Economy::setInflation(double rate) {
    if (rate < 0.0) rate = 0.0;
    if (rate > 50.0) rate = 50.0;
    inflation = rate;
}

int Economy::getEmploymentRate() const {
    return employmentRate;
}

void Economy::setEmploymentRate(int rate) {
    if (rate < 0) rate = 0;
    if (rate > 100) rate = 100;
    employmentRate = rate;
}

int Economy::getCorruptionLevel() const {
    return corruptionLevel;
}

void Economy::setCorruptionLevel(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    corruptionLevel = level;
}

Treasury* Economy::getTreasury() const {
    return treasury;
}

Bank* Economy::getBank() const {
    return bank;
}

void Economy::setBank(Bank* newBank) {
    bank = newBank;
}

double Economy::getTariffRate() const {
    return tariffRate;
}

void Economy::setTariffRate(double rate) {
    if (rate < 0.0) rate = 0.0;
    if (rate > 50.0) rate = 50.0;
    tariffRate = rate;
}

// Resource management
void Economy::addResource(Resource* resource) {
    if (!resource) {
        throw invalid_argument("Cannot add null resource");
    }

    // Check if we need to resize the array
    if (numResources >= maxResources) {
        // Create a new, larger array
        int newMaxResources = maxResources * 2;
        Resource** newResources = new Resource * [newMaxResources];

        // Copy existing resources to the new array
        for (int i = 0; i < numResources; i++) {
            newResources[i] = resources[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numResources; i < newMaxResources; i++) {
            newResources[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] resources;
        resources = newResources;
        maxResources = newMaxResources;
    }

    // Add the new resource
    resources[numResources++] = resource;
}

void Economy::removeResource(int index) {
    if (index < 0 || index >= numResources) {
        throw out_of_range("Resource index out of range");
    }

    // Delete the resource object
    delete resources[index];

    // Shift remaining elements
    for (int i = index; i < numResources - 1; i++) {
        resources[i] = resources[i + 1];
    }

    // Set the last position to nullptr and decrement count
    resources[--numResources] = nullptr;
}

Resource* Economy::getResource(int index) const {
    if (index < 0 || index >= numResources) {
        throw out_of_range("Resource index out of range");
    }
    return resources[index];
}

Resource* Economy::getResourceByType(ResourceType type) const {
    for (int i = 0; i < numResources; i++) {
        if (resources[i] && resources[i]->getType() == type) {
            return resources[i];
        }
    }
    return nullptr;
}

int Economy::getNumResources() const {
    return numResources;
}

// Trade system
bool Economy::trade(ResourceType sellResource, int sellAmount, ResourceType buyResource, int buyAmount) {
    // Find the resources
    Resource* sellItem = getResourceByType(sellResource);
    Resource* buyItem = getResourceByType(buyResource);

    if (!sellItem || !buyItem) {
        return false;
    }

    // Check if we have enough of the selling resource
    if (sellItem->getAmount() < sellAmount) {
        return false;
    }

    // Execute the trade
    sellItem->setAmount(sellItem->getAmount() - sellAmount);
    buyItem->setAmount(buyItem->getAmount() + buyAmount);

    // Trade success bonus - small chance to increase trade level
    if (randomInt(1, 100) <= 10) {
        setTradeLevel(min(100, tradeLevel + 1));
    }

    return true;
}

// Economic calculations
int Economy::calculateProductionOutput() const {
    // Base production output
    int output = productionLevel;

    // Adjust based on employment rate
    output = (output * employmentRate) / 100;

    // Adjust based on market stability
    output = (output * marketStability) / 100;

    // Reduction due to corruption
    int corruptionReduction = (output * corruptionLevel) / 100;
    output -= corruptionReduction;

    return max(0, output);
}

void Economy::calculateInflation() {
    // Base inflation adjustment
    double baseChange = randomDouble(-0.5, 0.5);

    // Market stability effect (more stable = less inflation)
    double stabilityEffect = (50 - marketStability) / 100.0;

    // Production/consumption ratio effect
    double productionRatio = 1.0;
    int totalConsumption = 0;
    int totalProduction = 0;

    for (int i = 0; i < numResources; i++) {
        if (resources[i]) {
            totalConsumption += resources[i]->getConsumptionRate();
            totalProduction += resources[i]->getGatherRate();
        }
    }

    if (totalProduction > 0) {
        productionRatio = (double)totalConsumption / totalProduction;
    }

    // If consumption exceeds production, inflation increases
    double productionEffect = (productionRatio - 1.0) * 2.0;
    if (productionEffect < 0) productionEffect = 0;

    // Treasury effect - large gold reserves can reduce inflation
    double treasuryEffect = 0;
    if (treasury && treasury->getGold() > 5000) {
        treasuryEffect = -0.5; // Negative means inflation reduction
    }

    // Calculate new inflation rate
    double newInflation = inflation + baseChange + stabilityEffect + productionEffect + treasuryEffect;

    // Ensure inflation stays within bounds
    setInflation(newInflation);
}

// Update method required by Entity base class
void Economy::update() {
    if (!getIsActive()) return;

    // Update resources
    for (int i = 0; i < numResources; i++) {
        if (resources[i]) {
            resources[i]->update();
        }
    }

    // Update market stability based on random fluctuations
    int stabilityChange = randomInt(-5, 5);
    marketStability = max(0, min(100, marketStability + stabilityChange));

    // Update inflation
    calculateInflation();

    // Update employment based on production and market stability
    int employmentChange = randomInt(-2, 2);
    employmentChange += (productionLevel - 50) / 10; // Higher production increases employment
    employmentChange += (marketStability - 50) / 10; // Higher stability increases employment

    employmentRate = max(0, min(100, employmentRate + employmentChange));

    // Update corruption level (small random changes)
    int corruptionChange = randomInt(-1, 2);
    corruptionLevel = max(0, min(100, corruptionLevel + corruptionChange));

    // Process trade routes
    processTradeRoutes();

    // Update treasury
    if (treasury) {
        treasury->update();
    }
}

// Save economy data to file
void Economy::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save Economy: File not open");
    }

    // Call base class save first
    Entity::save(file);

    // Save Economy-specific attributes
    file << productionLevel << endl;
    file << tradeLevel << endl;
    file << taxRate << endl;
    file << tariffRate << endl;
    file << marketStability << endl;
    file << inflation << endl;
    file << employmentRate << endl;
    file << corruptionLevel << endl;

    // Save number of resources
    file << numResources << endl;

    // Save each resource
    for (int i = 0; i < numResources; i++) {
        resources[i]->save(file);
    }

    // Save number of trade routes
    file << numTradeRoutes << endl;

    // Trade routes are recreated dynamically, so we don't save them

    // Save treasury
    treasury->save(file);
}

// Load economy data from file
void Economy::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load Economy: File not open");
    }

    // Call base class load first
    Entity::load(file);

    // Load Economy-specific attributes
    file >> productionLevel;
    file >> tradeLevel;
    file >> taxRate;
    file >> tariffRate;
    file >> marketStability;
    file >> inflation;
    file >> employmentRate;
    file >> corruptionLevel;

    // Clean up existing resources
    for (int i = 0; i < numResources; i++) {
        delete resources[i];
        resources[i] = nullptr;
    }
    numResources = 0;

    // Load number of resources
    int loadNumResources;
    file >> loadNumResources;
    file.ignore(); // Skip newline

    // Load each resource
    for (int i = 0; i < loadNumResources; i++) {
        Resource* resource = new Resource("Temp", FOOD);
        resource->load(file);
        addResource(resource);
    }

    // Clean up existing trade routes
    for (int i = 0; i < numTradeRoutes; i++) {
        delete tradeRoutes[i];
        tradeRoutes[i] = nullptr;
    }

    // Load number of trade routes
    int loadNumTradeRoutes;
    file >> loadNumTradeRoutes;
    numTradeRoutes = 0; // Trade routes will be recreated dynamically during gameplay

    // Load treasury
    treasury->load(file);
}

// Trade route management
void Economy::addTradeRoute(Kingdom* targetKingdom, ResourceType exportResource,
    int exportAmount, ResourceType importResource, int importAmount) {
    if (!targetKingdom) {
        throw invalid_argument("Cannot create trade route with null kingdom");
    }

    // Check if we need to resize the array
    if (numTradeRoutes >= maxTradeRoutes) {
        // Create a new, larger array
        int newMaxTradeRoutes = maxTradeRoutes * 2;
        TradeRoute** newTradeRoutes = new TradeRoute * [newMaxTradeRoutes];

        // Copy existing trade routes to the new array
        for (int i = 0; i < numTradeRoutes; i++) {
            newTradeRoutes[i] = tradeRoutes[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numTradeRoutes; i < newMaxTradeRoutes; i++) {
            newTradeRoutes[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] tradeRoutes;
        tradeRoutes = newTradeRoutes;
        maxTradeRoutes = newMaxTradeRoutes;
    }

    // Create and add the new trade route
    TradeRoute* newRoute = new TradeRoute();
    newRoute->targetKingdom = targetKingdom;
    newRoute->exportResource = exportResource;
    newRoute->exportAmount = exportAmount;
    newRoute->importResource = importResource;
    newRoute->importAmount = importAmount;
    newRoute->profitMargin = 1.0 + (tradeLevel / 100.0); // Higher trade level = better profit margin
    newRoute->isActive = true;

    tradeRoutes[numTradeRoutes++] = newRoute;

    // Trade routes improve trade level over time
    if (tradeLevel < 100) {
        tradeLevel += 1;
    }
}

// Overloaded version that takes a kingdom name instead of pointer
void Economy::addTradeRoute(const string& targetKingdomName, ResourceType exportResource,
    int exportAmount, ResourceType importResource, int importAmount) {
    // Check if we need to resize the array
    if (numTradeRoutes >= maxTradeRoutes) {
        // Create a new, larger array
        int newMaxTradeRoutes = maxTradeRoutes * 2;
        TradeRoute** newTradeRoutes = new TradeRoute * [newMaxTradeRoutes];

        // Copy existing trade routes to the new array
        for (int i = 0; i < numTradeRoutes; i++) {
            newTradeRoutes[i] = tradeRoutes[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numTradeRoutes; i < newMaxTradeRoutes; i++) {
            newTradeRoutes[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] tradeRoutes;
        tradeRoutes = newTradeRoutes;
        maxTradeRoutes = newMaxTradeRoutes;
    }

    // Create and add the new trade route
    TradeRoute* newRoute = new TradeRoute();
    newRoute->targetKingdom = nullptr; // No actual kingdom pointer, just using the name
    newRoute->targetKingdomName = targetKingdomName; // Store the kingdom name
    newRoute->exportResource = exportResource;
    newRoute->exportAmount = exportAmount;
    newRoute->importResource = importResource;
    newRoute->importAmount = importAmount;
    newRoute->profitMargin = 1.0 + (tradeLevel / 100.0); // Higher trade level = better profit margin
    newRoute->isActive = true;

    tradeRoutes[numTradeRoutes++] = newRoute;

    // Trade routes improve trade level over time
    if (tradeLevel < 100) {
        tradeLevel += 1;
    }
}

void Economy::removeTradeRoute(int index) {
    if (index < 0 || index >= numTradeRoutes) {
        throw out_of_range("Trade route index out of range");
    }

    // Delete the trade route object
    delete tradeRoutes[index];

    // Shift remaining elements
    for (int i = index; i < numTradeRoutes - 1; i++) {
        tradeRoutes[i] = tradeRoutes[i + 1];
    }

    // Set the last position to nullptr and decrement count
    tradeRoutes[--numTradeRoutes] = nullptr;

    // Removing trade routes can slightly decrease trade level
    if (tradeLevel > 0) {
        tradeLevel -= 1;
    }
}

int Economy::getNumTradeRoutes() const {
    return numTradeRoutes;
}

int Economy::getMaxTradeRoutes() const {
    // Base max routes depends on trade level
    int baseMax = 3 + (tradeLevel / 20);

    // Cap at 10 trade routes
    return min(10, baseMax);
}

void Economy::displayTradeRoutes() const {
    cout << "=== ACTIVE TRADE ROUTES ===\n" << endl;

    if (numTradeRoutes == 0) {
        cout << "No active trade routes." << endl;
        return;
    }

    for (int i = 0; i < numTradeRoutes; i++) {
        TradeRoute* route = tradeRoutes[i];
        if (route) {
            // Display trade partner
            cout << (i + 1) << ". Trade with: ";
            if (route->targetKingdom) {
                cout << route->targetKingdom->getName();
            }
            else if (!route->targetKingdomName.empty()) {
                cout << route->targetKingdomName;
            }
            else {
                cout << "Foreign Kingdom"; // Generic name when both Kingdom* and name are null/empty
            }
            cout << endl;

            // Display export details
            cout << "   Export: " << route->exportAmount << " ";
            switch (route->exportResource) {
            case FOOD: cout << "Food"; break;
            case WOOD: cout << "Wood"; break;
            case STONE: cout << "Stone"; break;
            case GOLD: cout << "Gold"; break;
            case IRON: cout << "Iron"; break;
            default: cout << "Unknown Resource"; break;
            }
            cout << endl;

            // Display import details
            cout << "   Import: " << route->importAmount << " ";
            switch (route->importResource) {
            case FOOD: cout << "Food"; break;
            case WOOD: cout << "Wood"; break;
            case STONE: cout << "Stone"; break;
            case GOLD: cout << "Gold"; break;
            case IRON: cout << "Iron"; break;
            default: cout << "Unknown Resource"; break;
            }
            cout << endl;

            // Display profit margin
            cout << "   Profit Margin: " << (int)((route->profitMargin - 1.0) * 100) << "%" << endl;
            cout << "   Status: " << (route->isActive ? "Active" : "Inactive") << endl;
            cout << endl;
        }
    }
}

void Economy::processTradeRoutes() {
    for (int i = 0; i < numTradeRoutes; i++) {
        TradeRoute* route = tradeRoutes[i];
        if (route && route->isActive) {
            // Find our export resource
            Resource* exportRes = getResourceByType(route->exportResource);
            if (!exportRes || exportRes->getAmount() < route->exportAmount) {
                // Not enough of export resource, skip this trade route this turn
                continue;
            }

            // Reduce our export resource
            exportRes->setAmount(exportRes->getAmount() - route->exportAmount);

            // Find our import resource
            Resource* importRes = getResourceByType(route->importResource);
            if (!importRes) {
                // We don't have this resource type, skip this trade route
                continue;
            }

            // Apply tariff to determine actual import amount
            int actualImportAmount = route->importAmount;
            if (tariffRate > 0) {
                int tariffCost = (int)(actualImportAmount * tariffRate);
                actualImportAmount -= tariffCost;

                // Add tariff income to treasury
                if (treasury) {
                    treasury->earn(tariffCost * 2); // Convert resource units to gold value
                }
            }

            // Apply trade level bonus
            actualImportAmount = (int)(actualImportAmount * route->profitMargin);

            // Increase our import resource
            importRes->setAmount(importRes->getAmount() + actualImportAmount);

            // Trade success improves trade level
            if (randomInt(1, 100) <= 10) {
                if (tradeLevel < 100) {
                    tradeLevel += 1;
                }
            }
        }
    }
}