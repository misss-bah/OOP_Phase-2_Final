#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Disease::Disease(const string& name, const string& description, int severity, int infectivity, int duration, DiseaseType type)
    : EnvironmentalEffect(name, description, severity, duration), type(type), infectivity(infectivity),
    mortalityRate(severity * 2), currentInfected(0) {
    // Validate infectivity
    if (infectivity < 0) {
        this->infectivity = 0;
    }
    else if (infectivity > 100) {
        this->infectivity = 100;
    }
}

// Destructor
Disease::~Disease() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
DiseaseType Disease::getType() const {
    return type;
}

void Disease::setType(DiseaseType newType) {
    type = newType;
}

int Disease::getInfectivity() const {
    return infectivity;
}

int Disease::getMortalityRate() const {
    return mortalityRate;
}

int Disease::getCurrentInfected() const {
    return currentInfected;
}

void Disease::setCurrentInfected(int infected) {
    if (infected < 0) {
        throw invalid_argument("Number of infected cannot be negative");
    }
    currentInfected = infected;
}

// Begin a disease outbreak with a specified number of initial infections
void Disease::outbreak(int initialInfected) {
    if (initialInfected < 0) {
        throw invalid_argument("Initial infected must be non-negative");
    }

    currentInfected = initialInfected;
    turnsActive = 0;

    cout << "A " << name << " outbreak has begun with " << initialInfected << " initial cases!" << endl;
}

// Spread the disease through the population
void Disease::spread(Population* population, Weather* currentWeather) {
    if (!population) {
        throw invalid_argument("Cannot spread disease: Population is null");
    }

    int totalPop = population->getTotalPopulation();
    if (totalPop <= 0) return; // No population to infect

    // Calculate spread factors
    int weatherModifier = currentWeather ? currentWeather->getDiseaseModifier() : 0;
    int susceptibility = population->getDiseaseSusceptibility();

    // Modified infectivity based on weather and population health
    int modifiedInfectivity = infectivity + weatherModifier;
    modifiedInfectivity = max(5, min(95, modifiedInfectivity)); // Keep between 5% and 95%

    // Calculate new infections
    int newInfections = (totalPop - currentInfected) * modifiedInfectivity * susceptibility / 10000;

    // Apply random variations (±20%)
    newInfections = newInfections * (80 + randomInt(0, 40)) / 100;

    // Update infected count
    currentInfected += newInfections;
    if (currentInfected > totalPop) {
        currentInfected = totalPop;
    }

    cout << "The " << name << " has spread to " << newInfections << " new people. ";
    cout << "Total infected: " << currentInfected << " (" << (currentInfected * 100 / totalPop) << "% of population)" << endl;
}

// Calculate deaths from the disease
int Disease::calculateDeaths() {
    if (currentInfected <= 0) return 0;

    // Formula: deaths = infected * mortality rate / 100
    int deaths = currentInfected * mortalityRate / 100;

    // Apply random variation (±10%)
    deaths = deaths * (90 + randomInt(0, 20)) / 100;

    // Update infected count
    currentInfected -= deaths;
    if (currentInfected < 0) currentInfected = 0;

    return deaths;
}

// Apply disease effects to a kingdom
void Disease::applyEffects(Kingdom* kingdom) {
    if (!kingdom) {
        throw invalid_argument("Cannot apply disease effects: Kingdom is null");
    }

    // Get required components
    Population* population = kingdom->getPopulation();
    Weather* currentWeather = kingdom->getCurrentWeather();

    if (!population) {
        throw runtime_error("Cannot apply disease effects: Population not found in kingdom");
    }

    // Spread the disease
    spread(population, currentWeather);

    // Calculate deaths
    int deaths = calculateDeaths();

    // Apply deaths to population
    if (deaths > 0) {
        int oldPopulation = population->getTotalPopulation();

        // Apply disease effects to each social class
        for (int i = 0; i < population->getNumClasses(); i++) {
            SocialClass* socialClass = population->getSocialClass(i);
            if (socialClass) {
                socialClass->applyDiseaseEffect(*this);
            }
        }

        int newPopulation = population->getTotalPopulation();

        cout << "The " << name << " has claimed approximately " << (oldPopulation - newPopulation)
            << " lives in the kingdom." << endl;
    }

    // Disease also affects economy (reduced productivity)
    Economy* economy = kingdom->getEconomy();
    if (economy) {
        // Reduce production based on severity and infection rate
        int productionReduction = (severity * currentInfected) / (population->getTotalPopulation() * 5 + 1);
        economy->setProductionLevel(economy->getProductionLevel() - productionReduction);
    }

    // Natural recovery - some infected get better each turn
    int recoveries = currentInfected * (100 - severity) / 300;
    currentInfected -= recoveries;
    if (currentInfected < 0) currentInfected = 0;

    // Check if disease has run its course
    if (turnsActive >= duration || currentInfected == 0) {
        setIsActive(false);
        cout << "The " << name << " outbreak has ended." << endl;
    }
}

// Update method required by Entity base class
void Disease::update() {
    if (!isActive) return;

    incrementTurnsActive();

    // Disease might naturally become less severe over time
    if (randomInt(1, 10) == 1) {
        severity = max(1, severity - 1);
    }

    // Chance for disease to naturally end before duration
    if (turnsActive > duration / 2 && randomInt(1, 10) == 1) {
        setIsActive(false);
    }
}

// Save disease data to file
void Disease::save(ofstream& file) const {
    // Call base class save first
    EnvironmentalEffect::save(file);

    // Save Disease-specific attributes
    file << static_cast<int>(type) << endl;
    file << infectivity << endl;
    file << mortalityRate << endl;
    file << currentInfected << endl;
}

// Load disease data from file
void Disease::load(ifstream& file) {
    // Call base class load first
    EnvironmentalEffect::load(file);

    // Load Disease-specific attributes
    int typeInt;
    file >> typeInt;
    type = static_cast<DiseaseType>(typeInt);
    file >> infectivity;
    file >> mortalityRate;
    file >> currentInfected;
    file.ignore(); // Skip newline
}
