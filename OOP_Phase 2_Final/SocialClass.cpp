#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor implementation
SocialClass::SocialClass(const string& name, int initialPopulation, double initialTaxRate)
    : Human(name), population(initialPopulation), taxRate(initialTaxRate), diseaseResistance(50) {
    // Validate parameters
    if (initialPopulation < 0) {
        throw invalid_argument("Population cannot be negative");
    }

    if (initialTaxRate < 0.0 || initialTaxRate > 1.0) {
        throw invalid_argument("Tax rate must be between 0.0 and 1.0");
    }
}

// Destructor
SocialClass::~SocialClass() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
int SocialClass::getPopulation() const {
    return population;
}

void SocialClass::setPopulation(int newPopulation) {
    if (newPopulation < 0) {
        throw invalid_argument("Population cannot be negative");
    }
    population = newPopulation;
}

double SocialClass::getTaxRate() const {
    return taxRate;
}

void SocialClass::setTaxRate(double newRate) {
    if (newRate < 0.0 || newRate > 1.0) {
        throw invalid_argument("Tax rate must be between 0.0 and 1.0");
    }
    taxRate = newRate;

    // Tax rate affects happiness
    if (taxRate > 0.5) {
        // High tax rates reduce happiness
        adjustHappiness(-(static_cast<int>((taxRate - 0.5) * 20)));
    }
}

int SocialClass::calculateTaxRevenue() const {
    // Base tax revenue is population * tax rate * 10 (arbitrary value)
    return static_cast<int>(population * taxRate * 10);
}

int SocialClass::getDiseaseResistance() const {
    return diseaseResistance;
}

void SocialClass::setDiseaseResistance(int resistance) {
    if (resistance < 0) resistance = 0;
    if (resistance > 100) resistance = 100;
    diseaseResistance = resistance;
}

// Override the isUnrestLikely method from Human
bool SocialClass::isUnrestLikely() const {
    // Base likelihood from Human class
    bool baseUnrest = Human::isUnrestLikely();

    // Additional factors for social classes
    // High tax rates or low health increase unrest chances
    return baseUnrest || taxRate > 0.7 || health < 20;
}

// Override the applyDiseaseEffect method from Human
void SocialClass::applyDiseaseEffect(const Disease& disease) {
    // Call base implementation first
    Human::applyDiseaseEffect(disease);

    // Calculate population loss due to disease
    int severityAdjusted = disease.getSeverity() * (100 - diseaseResistance) / 100;
    int mortalityRate = disease.getMortalityRate() + (100 - health) / 20; // Health impacts mortality

    // Calculate deaths (based on severity, resistance, and population)
    int deaths = (population * severityAdjusted * mortalityRate) / 10000;

    // Apply population reduction
    population -= deaths;
    if (population < 0) population = 0;

    // Deaths impact happiness negatively
    if (deaths > 0) {
        adjustHappiness(-min(10, deaths / 10));
    }
}

// Override the applyWeatherEffect method from Human
void SocialClass::applyWeatherEffect(const Weather& weather) {
    // Call base implementation first
    Human::applyWeatherEffect(weather);

    // Extreme weather may cause deaths in the population
    if (weather.getIsExtreme()) {
        int deaths = (population * (weather.getSeverity() - diseaseResistance / 10)) / 1000;
        deaths = max(0, deaths); // Ensure no negative deaths

        // Apply population reduction
        population -= deaths;
        if (population < 0) population = 0;

        // Deaths impact happiness negatively
        if (deaths > 0) {
            adjustHappiness(-min(5, deaths / 20));
        }
    }
}

// Update method required by Entity base class
void SocialClass::update() {
    // Call base class update
    Human::update();

    // Natural population growth based on health and happiness
    double growthFactor = (health / 100.0) * (happiness / 100.0);
    int growth = static_cast<int>(population * growthFactor * 0.01); // 1% max growth

    // Apply growth
    population += growth;

    // Tax rate affects loyalty over time
    if (taxRate > 0.6) {
        adjustLoyalty(-1);
    }
    else if (taxRate < 0.2) {
        adjustLoyalty(1);
    }
}

// Save social class data to file
void SocialClass::save(ofstream& file) const {
    // Call base class save first
    Human::save(file);

    // Save SocialClass-specific attributes
    file << population << endl;
    file << taxRate << endl;
    file << diseaseResistance << endl;
}

// Load social class data from file
void SocialClass::load(ifstream& file) {
    // Call base class load first
    Human::load(file);

    // Load SocialClass-specific attributes
    file >> population;
    file >> taxRate;
    file >> diseaseResistance;
    file.ignore(); // Skip newline
}
