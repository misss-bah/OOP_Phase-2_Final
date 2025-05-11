#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Population::Population(int initialPopulation)
    : totalPopulation(initialPopulation), growthRate(5), foodConsumptionPerCapita(2),
    healthLevel(70), unrestActive(false), diseaseSusceptibility(50) {

    // Initialize dynamic array for social classes
    maxClasses = 5;
    numClasses = 0;
    classes = new SocialClass * [maxClasses];

    for (int i = 0; i < maxClasses; i++) {
        classes[i] = nullptr;
    }

    // Create default social classes if initialPopulation > 0
    if (initialPopulation > 0) {
        // Create default social classes with distribution percentages
        SocialClass* peasants = new SocialClass("Peasants", initialPopulation * 70 / 100, 0.15);
        addSocialClass(peasants);

        SocialClass* merchants = new SocialClass("Merchants", initialPopulation * 20 / 100, 0.25);
        addSocialClass(merchants);

        SocialClass* nobles = new SocialClass("Nobles", initialPopulation * 10 / 100, 0.35);
        addSocialClass(nobles);
    }
}

// Destructor
Population::~Population() {
    // Clean up dynamically allocated social classes
    for (int i = 0; i < numClasses; i++) {
        delete classes[i];
    }

    // Clean up the array itself
    delete[] classes;
}

// Add a new social class to the population
void Population::addSocialClass(SocialClass* newClass) {
    if (!newClass) {
        throw invalid_argument("Cannot add null social class");
    }

    // Check if we need to resize the array
    if (numClasses >= maxClasses) {
        // Create a new, larger array
        int newMaxClasses = maxClasses * 2;
        SocialClass** newClasses = new SocialClass * [newMaxClasses];

        // Copy existing classes to the new array
        for (int i = 0; i < numClasses; i++) {
            newClasses[i] = classes[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numClasses; i < newMaxClasses; i++) {
            newClasses[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] classes;
        classes = newClasses;
        maxClasses = newMaxClasses;
    }

    // Add the new class
    classes[numClasses++] = newClass;

    // Update total population
    recalculateTotalPopulation();
}

// Remove a social class at a specific index
void Population::removeClassAt(int index) {
    if (index < 0 || index >= numClasses) {
        throw out_of_range("Social class index out of range");
    }

    // Delete the class object
    delete classes[index];

    // Shift remaining elements
    for (int i = index; i < numClasses - 1; i++) {
        classes[i] = classes[i + 1];
    }

    // Set the last position to nullptr and decrement count
    classes[--numClasses] = nullptr;

    // Update total population
    recalculateTotalPopulation();
}

// Recalculate the total population by summing all social classes
void Population::recalculateTotalPopulation() {
    totalPopulation = 0;
    for (int i = 0; i < numClasses; i++) {
        if (classes[i]) {
            totalPopulation += classes[i]->getPopulation();
        }
    }
}

// Getters and setters
int Population::getTotalPopulation() const {
    return totalPopulation;
}

int Population::getGrowthRate() const {
    return growthRate;
}

void Population::setGrowthRate(int rate) {
    if (rate < -10) rate = -10;
    if (rate > 20) rate = 20;
    growthRate = rate;
}

bool Population::isUnrestActive() const {
    return unrestActive;
}

int Population::getHealthLevel() const {
    return healthLevel;
}

void Population::setHealthLevel(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    healthLevel = level;
}

int Population::getDiseaseSusceptibility() const {
    return diseaseSusceptibility;
}

void Population::setDiseaseSusceptibility(int susceptibility) {
    if (susceptibility < 0) susceptibility = 0;
    if (susceptibility > 100) susceptibility = 100;
    diseaseSusceptibility = susceptibility;
}

SocialClass* Population::getSocialClass(int index) const {
    if (index < 0 || index >= numClasses) {
        throw out_of_range("Social class index out of range");
    }
    return classes[index];
}

int Population::getNumClasses() const {
    return numClasses;
}

int Population::getFoodConsumptionPerCapita() const {
    return foodConsumptionPerCapita;
}

void Population::setFoodConsumptionPerCapita(int consumption) {
    if (consumption < 1) consumption = 1;
    if (consumption > 10) consumption = 10;
    foodConsumptionPerCapita = consumption;
}

// Update the population based on available food and other factors
void Population::update(int availableFood) {
    // First, calculate required food
    int requiredFood = totalPopulation * foodConsumptionPerCapita;

    // Calculate food shortage if any
    int foodShortage = max(0, requiredFood - availableFood);
    double foodShortageRatio = 0.0;

    if (requiredFood > 0) {
        foodShortageRatio = static_cast<double>(foodShortage) / requiredFood;
    }

    // Iterate through all social classes
    for (int i = 0; i < numClasses; i++) {
        if (!classes[i]) continue;

        // Update class
        classes[i]->update();

        // Handle food shortages - apply effects based on social hierarchy
        // Lower classes (higher indices) suffer more from shortage
        if (foodShortageRatio > 0.0) {
            double classFoodShortage = foodShortageRatio * (1.0 + static_cast<double>(i) / numClasses);
            classFoodShortage = min(1.0, classFoodShortage); // Cap at 100% shortage

            // Happiness and health penalties based on food shortage
            int happinessReduction = static_cast<int>(classFoodShortage * 30);
            int healthReduction = static_cast<int>(classFoodShortage * 15);

            classes[i]->adjustHappiness(-happinessReduction);
            classes[i]->adjustHealth(-healthReduction);

            // Extreme food shortage causes population decline
            if (classFoodShortage > 0.5) {
                int starvationDeaths = static_cast<int>(classes[i]->getPopulation() * (classFoodShortage - 0.5) * 0.1);
                int newPopulation = max(0, classes[i]->getPopulation() - starvationDeaths);
                classes[i]->setPopulation(newPopulation);

                if (starvationDeaths > 0) {
                    cout << starvationDeaths << " people died of starvation in the " << classes[i]->getName() << " class." << endl;
                }
            }
        }

        // Check for unrest
        if (classes[i]->isUnrestLikely()) {
            // The larger the class, the more impact its unrest has
            double unrestProbability = static_cast<double>(classes[i]->getPopulation()) / totalPopulation * 0.5;

            if (randomDouble(0.0, 1.0) < unrestProbability) {
                unrestActive = true;
                cout << "Unrest has broken out among the " << classes[i]->getName() << "!" << endl;
            }
        }
    }

    // Recalculate total population
    recalculateTotalPopulation();

    // Calculate new average health level
    int totalHealth = 0;
    for (int i = 0; i < numClasses; i++) {
        if (classes[i]) {
            totalHealth += classes[i]->getHealth() * classes[i]->getPopulation();
        }
    }

    healthLevel = (totalPopulation > 0) ? (totalHealth / totalPopulation) : 50;

    // Update disease susceptibility based on health level
    diseaseSusceptibility = 100 - healthLevel / 2;

    // Update growth rate based on health, food, and whether unrest is active
    int baseGrowthRate = 5; // 0.5% base growth rate

    // Health affects growth
    baseGrowthRate += (healthLevel - 50) / 10;

    // Food affects growth
    if (foodShortageRatio > 0.0) {
        baseGrowthRate -= static_cast<int>(foodShortageRatio * 10);
    }

    // Unrest reduces growth
    if (unrestActive) {
        baseGrowthRate -= 5;
    }

    // Constrain growth rate
    growthRate = max(-10, min(20, baseGrowthRate));

    // Apply natural population growth/decline to each class
    for (int i = 0; i < numClasses; i++) {
        if (classes[i]) {
            int classGrowth = static_cast<int>(classes[i]->getPopulation() * growthRate / 1000.0);
            classes[i]->setPopulation(max(0, classes[i]->getPopulation() + classGrowth));
        }
    }

    // Recalculate total population again after growth
    recalculateTotalPopulation();

    // Chance for unrest to subside naturally
    if (unrestActive && randomInt(1, 5) == 1) {
        unrestActive = false;
        cout << "The unrest in the kingdom has subsided." << endl;
    }
}

// Handle effects of events on population
void Population::handleEvent(const Event& event) {
    // Apply population effect to all classes proportionally
    int effect = event.getPopulationEffect();

    if (effect == 0) return; // No population effect

    // Apply effects to each class proportionally
    for (int i = 0; i < numClasses; i++) {
        if (!classes[i]) continue;

        // Effect can be positive (population increase) or negative (decrease)
        int population = classes[i]->getPopulation();
        int change = static_cast<int>(population * effect / 100.0);

        // Apply change
        classes[i]->setPopulation(max(0, population + change));

        // Effect on happiness and health
        if (effect > 0) {
            classes[i]->adjustHappiness(min(10, effect / 2));
            classes[i]->adjustHealth(min(5, effect / 4));
        }
        else {
            classes[i]->adjustHappiness(max(-10, effect / 2));
            classes[i]->adjustHealth(max(-5, effect / 4));
        }
    }

    // Recalculate total population
    recalculateTotalPopulation();

    // Events can trigger or resolve unrest
    if (effect < -10 && !unrestActive && randomInt(1, 4) == 1) {
        unrestActive = true;
        cout << "The event has triggered unrest among the population!" << endl;
    }
    else if (effect > 10 && unrestActive && randomInt(1, 3) == 1) {
        unrestActive = false;
        cout << "The positive event has helped resolve the unrest!" << endl;
    }
}

// Handle effects of weather on population
void Population::handleWeatherEffects(const Weather& weather) {
    // Apply weather effects to each social class
    for (int i = 0; i < numClasses; i++) {
        if (classes[i]) {
            classes[i]->applyWeatherEffect(weather);
        }
    }

    // Recalculate total population
    recalculateTotalPopulation();

    // Extreme weather can trigger unrest
    if (weather.getIsExtreme() && !unrestActive && randomInt(1, 4) == 1) {
        unrestActive = true;
        cout << "The " << weather.getName() << " has caused unrest among the population!" << endl;
    }
}

// Handle effects of disease on population
void Population::handleDiseaseEffects(const Disease& disease) {
    // Apply disease effects to each social class
    for (int i = 0; i < numClasses; i++) {
        if (classes[i]) {
            classes[i]->applyDiseaseEffect(disease);
        }
    }

    // Recalculate total population
    recalculateTotalPopulation();

    // Severe diseases affect health level and can trigger unrest
    if (disease.getSeverity() > 3) {
        healthLevel = max(10, healthLevel - 5);

        if (!unrestActive && randomInt(1, 3) == 1) {
            unrestActive = true;
            cout << "The " << disease.getName() << " outbreak has caused unrest among the population!" << endl;
        }
    }
}

// Save population data to file
void Population::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save Population: File not open");
    }

    file << totalPopulation << endl;
    file << growthRate << endl;
    file << foodConsumptionPerCapita << endl;
    file << healthLevel << endl;
    file << unrestActive << endl;
    file << diseaseSusceptibility << endl;

    // Save number of classes
    file << numClasses << endl;

    // Save each social class
    for (int i = 0; i < numClasses; i++) {
        if (classes[i]) {
            classes[i]->save(file);
        }
    }
}

// Load population data from file
void Population::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load Population: File not open");
    }

    file >> totalPopulation;
    file >> growthRate;
    file >> foodConsumptionPerCapita;
    file >> healthLevel;
    file >> unrestActive;
    file >> diseaseSusceptibility;

    // Load number of classes
    int loadedNumClasses;
    file >> loadedNumClasses;
    file.ignore(); // Skip newline

    // Delete existing classes
    for (int i = 0; i < numClasses; i++) {
        delete classes[i];
        classes[i] = nullptr;
    }

    numClasses = 0;

    // Resize array if needed
    if (loadedNumClasses > maxClasses) {
        delete[] classes;
        maxClasses = loadedNumClasses;
        classes = new SocialClass * [maxClasses];

        for (int i = 0; i < maxClasses; i++) {
            classes[i] = nullptr;
        }
    }

    // Load each social class
    for (int i = 0; i < loadedNumClasses; i++) {
        SocialClass* newClass = new SocialClass("", 0, 0.0);
        newClass->load(file);
        addSocialClass(newClass);
    }
}
