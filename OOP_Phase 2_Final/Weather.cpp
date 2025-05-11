#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Weather::Weather(const string& name, const string& description, int severity, WeatherType type)
    : EnvironmentalEffect(name, description, severity, randomInt(2, 5)), // Weather typically lasts 2-5 turns
    type(type), cropEffect(0), movementEffect(0), moraleEffect(0), diseaseModifier(0), isExtreme(false) {

    // Initialize effects based on weather type
    switch (type) {
    case SUNNY:
        cropEffect = 10; // Positive effect on crops
        movementEffect = 10; // Positive effect on movement
        moraleEffect = 5; // Positive effect on morale
        diseaseModifier = -5; // Reduces disease spread
        isExtreme = false;
        break;

    case RAINY:
        cropEffect = 5; // Moderate positive effect on crops
        movementEffect = -10; // Negative effect on movement
        moraleEffect = -2; // Slight negative effect on morale
        diseaseModifier = 10; // Increases disease spread
        isExtreme = (severity > 3); // Heavy rain is extreme
        break;

    case STORMY:
        cropEffect = -15; // Negative effect on crops
        movementEffect = -30; // Strong negative effect on movement
        moraleEffect = -10; // Strong negative effect on morale
        diseaseModifier = 0; // Neutral effect on disease
        isExtreme = true; // Storms are always extreme
        break;

    case SNOWY:
        cropEffect = -20; // Strong negative effect on crops
        movementEffect = -40; // Very strong negative effect on movement
        moraleEffect = -5; // Moderate negative effect on morale
        diseaseModifier = -10; // Reduces disease spread
        isExtreme = (severity > 2); // Heavy snow is extreme
        break;

    case DROUGHT:
        cropEffect = -30; // Very strong negative effect on crops
        movementEffect = 0; // Neutral effect on movement
        moraleEffect = -15; // Very strong negative effect on morale
        diseaseModifier = 5; // Slightly increases disease spread
        isExtreme = true; // Droughts are always extreme
        break;

    case FOGGY:
        cropEffect = -5; // Slight negative effect on crops
        movementEffect = -20; // Strong negative effect on movement
        moraleEffect = -3; // Slight negative effect on morale
        diseaseModifier = 5; // Slightly increases disease spread
        isExtreme = false;
        break;
    }

    // Scale effects by severity
    cropEffect *= severity;
    movementEffect *= severity;
    moraleEffect *= severity;
    diseaseModifier *= severity;

    // Normalize effects
    cropEffect /= 3;
    movementEffect /= 3;
    moraleEffect /= 3;
    diseaseModifier /= 3;
}

// Destructor
Weather::~Weather() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
WeatherType Weather::getType() const {
    return type;
}

void Weather::setType(WeatherType newType) {
    type = newType;
}

int Weather::getCropEffect() const {
    return cropEffect;
}

void Weather::setCropEffect(int effect) {
    cropEffect = effect;
}

int Weather::getMovementEffect() const {
    return movementEffect;
}

void Weather::setMovementEffect(int effect) {
    movementEffect = effect;
}

int Weather::getMoraleEffect() const {
    return moraleEffect;
}

void Weather::setMoraleEffect(int effect) {
    moraleEffect = effect;
}

int Weather::getDiseaseModifier() const {
    return diseaseModifier;
}

void Weather::setDiseaseModifier(int modifier) {
    diseaseModifier = modifier;
}

bool Weather::getIsExtreme() const {
    return isExtreme;
}

void Weather::setIsExtreme(bool extreme) {
    isExtreme = extreme;
}

// Generate random weather conditions
void Weather::generateRandomWeather() {
    // Generate a random type and severity
    int randomType = randomInt(0, 5);
    type = static_cast<WeatherType>(randomType);

    // Set severity (1-5) with more common weather being less severe
    int baseSeverity;
    switch (type) {
    case SUNNY:
    case RAINY:
        baseSeverity = randomInt(1, 3); // More common, less severe
        break;
    case FOGGY:
        baseSeverity = randomInt(1, 4);
        break;
    case SNOWY:
        baseSeverity = randomInt(2, 4);
        break;
    case STORMY:
    case DROUGHT:
        baseSeverity = randomInt(3, 5); // Less common, more severe
        break;
    default:
        baseSeverity = randomInt(1, 3);
    }

    setSeverity(baseSeverity);

    // Set duration based on severity
    setDuration(baseSeverity + randomInt(1, 3));

    // Reset turns active
    turnsActive = 0;

    // Update name and description based on type and severity
    string typeNames[] = { "Sunny", "Rainy", "Stormy", "Snowy", "Drought", "Foggy" };
    string severityDesc;

    switch (baseSeverity) {
    case 1: severityDesc = "Mild"; break;
    case 2: severityDesc = "Moderate"; break;
    case 3: severityDesc = "Strong"; break;
    case 4: severityDesc = "Severe"; break;
    case 5: severityDesc = "Extreme"; break;
    default: severityDesc = "Unusual";
    }

    // Set name and description
    setName(severityDesc + " " + typeNames[static_cast<int>(type)]);
    setDescription("The kingdom is experiencing " + severityDesc + " " + typeNames[static_cast<int>(type)] + " conditions.");

    // Reset and recalculate effects based on new weather
    Weather temp(getName(), getDescription(), getSeverity(), getType());
    cropEffect = temp.cropEffect;
    movementEffect = temp.movementEffect;
    moraleEffect = temp.moraleEffect;
    diseaseModifier = temp.diseaseModifier;
    isExtreme = temp.isExtreme;

    cout << "Weather has changed to: " << getName() << " - " << getDescription() << endl;
}

// Apply weather effects to a kingdom
void Weather::applyEffects(Kingdom* kingdom) {
    if (!kingdom) {
        throw invalid_argument("Cannot apply weather effects: Kingdom is null");
    }

    // Apply effects to population
    Population* population = kingdom->getPopulation();
    if (population) {
        population->handleWeatherEffects(*this);
    }

    // Apply effects to economy (crop production)
    Economy* economy = kingdom->getEconomy();
    if (economy) {
        // Adjust production level based on weather
        int currentProduction = economy->getProductionLevel();
        int adjustment = cropEffect / 10;
        economy->setProductionLevel(max(10, currentProduction + adjustment));

        // Adjust resource gathering rates based on weather
        for (int i = 0; i < economy->getNumResources(); i++) {
            Resource* resource = economy->getResource(i);
            if (resource) {
                // Food resources are most affected by weather
                if (resource->getType() == FOOD) {
                    int gatherRate = resource->getGatherRate();
                    int adjustment = gatherRate * cropEffect / 100;
                    resource->adjustGatherRate(adjustment);
                }
                // Other resources are less affected, mainly by movement effect
                else {
                    int gatherRate = resource->getGatherRate();
                    int adjustment = gatherRate * movementEffect / 200;
                    resource->adjustGatherRate(adjustment);
                }
            }
        }

        // Weather affects trade level
        if (isExtreme) {
            economy->setTradeLevel(max(10, economy->getTradeLevel() - 10));
        }
    }

    // Apply effects to army
    Army* army = kingdom->getArmy();
    if (army) {
        army->applyWeatherEffects(*this);
    }

    // Apply effects to leader (prepare for weather)
    Leader* leader = kingdom->getCurrentLeader();
    if (leader) {
        // Calculate leader's preparation level
        double preparedness = leader->calculateWeatherPreparedness();

        // Mitigate negative effects based on leader preparedness
        if (preparedness > 0.5 && isExtreme) {
            cout << leader->getName() << " has prepared the kingdom for the " << getName() << "." << endl;

            // Leadership experience from handling extreme weather
            if (isExtreme) {
                leader->incrementExperience();
            }
        }
    }
}

// Update method required by Entity base class
void Weather::update() {
    if (!isActive) return;

    incrementTurnsActive();

    // Weather gradually normalizes over time
    if (severity > 1 && randomInt(1, 3) == 1) {
        severity--;

        // Adjust effects as severity decreases
        cropEffect = cropEffect * 2 / 3;
        movementEffect = movementEffect * 2 / 3;
        moraleEffect = moraleEffect * 2 / 3;
        diseaseModifier = diseaseModifier * 2 / 3;

        // Extreme weather may become non-extreme
        if (severity <= 2) {
            isExtreme = false;
        }
    }

    // Check if weather has run its course
    if (turnsActive >= duration) {
        setIsActive(false);
    }
}

// Save weather data to file
void Weather::save(ofstream& file) const {
    // Call base class save first
    EnvironmentalEffect::save(file);

    // Save Weather-specific attributes
    file << static_cast<int>(type) << endl;
    file << cropEffect << endl;
    file << movementEffect << endl;
    file << moraleEffect << endl;
    file << diseaseModifier << endl;
    file << isExtreme << endl;
}

// Load weather data from file
void Weather::load(ifstream& file) {
    // Call base class load first
    EnvironmentalEffect::load(file);

    // Load Weather-specific attributes
    int typeInt;
    file >> typeInt;
    type = static_cast<WeatherType>(typeInt);
    file >> cropEffect;
    file >> movementEffect;
    file >> moraleEffect;
    file >> diseaseModifier;
    file >> isExtreme;
    file.ignore(); // Skip newline
}
