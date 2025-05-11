#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Kingdom::Kingdom(const string& name, bool isPlayerControlled)
    : name(name), isPlayerControlled(isPlayerControlled), turn(1), stabilityLevel(50) {

    // Initialize components
    population = new Population(1000);

    // Create a treasury
    Treasury* treasury = new Treasury("Royal Treasury");

    // Create economy with treasury
    economy = new Economy("Kingdom Economy", treasury);

    // Create army
    army = new Army("Royal Army");

    // Initialize leadership system
    leadershipSystem = new LeadershipSystem(this);

    // Create bank
    bank = new Bank("Royal Bank");

    // Initialize weather
    currentWeather = new Weather("Clear Skies", "The weather is fair.", 1, SUNNY);

    // No disease initially
    currentDisease = nullptr;

    // Initialize dynamic array for events
    maxEvents = 5;
    numEvents = 0;
    activeEvents = new Event * [maxEvents];

    for (int i = 0; i < maxEvents; i++) {
        activeEvents[i] = nullptr;
    }

    // Initialize current leader to null (must be set separately)
    currentLeader = nullptr;

    cout << "Kingdom " << name << " has been established." << endl;
}

// Destructor
Kingdom::~Kingdom() {
    // Clean up all components
    delete population;

    // Economy contains treasury, so treasury is deleted by Economy
    delete economy;

    delete army;
    delete leadershipSystem;

    // Current leader is managed by leadership system, don't delete here

    delete currentWeather;

    if (currentDisease) {
        delete currentDisease;
    }

    // Delete all active events
    for (int i = 0; i < numEvents; i++) {
        delete activeEvents[i];
    }

    // Delete the array itself
    delete[] activeEvents;

    delete bank;
}

// Getters and setters
string Kingdom::getName() const {
    return name;
}

void Kingdom::setName(const string& newName) {
    name = newName;
}

Population* Kingdom::getPopulation() const {
    return population;
}

Economy* Kingdom::getEconomy() const {
    return economy;
}

Army* Kingdom::getArmy() const {
    return army;
}

Leader* Kingdom::getCurrentLeader() const {
    return currentLeader;
}

void Kingdom::setCurrentLeader(Leader* leader) {
    currentLeader = leader;
}

LeadershipSystem* Kingdom::getLeadershipSystem() const {
    return leadershipSystem;
}

Weather* Kingdom::getCurrentWeather() const {
    return currentWeather;
}

void Kingdom::setCurrentWeather(Weather* weather) {
    if (currentWeather) {
        delete currentWeather;
    }
    currentWeather = weather;
}

Disease* Kingdom::getCurrentDisease() const {
    return currentDisease;
}

void Kingdom::setCurrentDisease(Disease* disease) {
    if (currentDisease) {
        delete currentDisease;
    }
    currentDisease = disease;
}

Bank* Kingdom::getBank() const {
    return bank;
}

int Kingdom::getTurn() const {
    return turn;
}

void Kingdom::incrementTurn() {
    turn++;
}

bool Kingdom::getIsPlayerControlled() const {
    return isPlayerControlled;
}

int Kingdom::getStabilityLevel() const {
    return stabilityLevel;
}

void Kingdom::setStabilityLevel(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    stabilityLevel = level;
}

// Add an event to the kingdom
void Kingdom::addEvent(Event* event) {
    if (!event) {
        throw invalid_argument("Cannot add null event");
    }

    // Check if we need to resize the array
    if (numEvents >= maxEvents) {
        // Create a new, larger array
        int newMaxEvents = maxEvents * 2;
        Event** newEvents = new Event * [newMaxEvents];

        // Copy existing events to the new array
        for (int i = 0; i < numEvents; i++) {
            newEvents[i] = activeEvents[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numEvents; i < newMaxEvents; i++) {
            newEvents[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] activeEvents;
        activeEvents = newEvents;
        maxEvents = newMaxEvents;
    }

    // Add the new event
    activeEvents[numEvents++] = event;

    // Apply the event's effects
    event->applyEffects(this);
}

// Remove an event at a specific index
void Kingdom::removeEvent(int index) {
    if (index < 0 || index >= numEvents) {
        throw out_of_range("Event index out of range");
    }

    // Delete the event object
    delete activeEvents[index];

    // Shift remaining elements
    for (int i = index; i < numEvents - 1; i++) {
        activeEvents[i] = activeEvents[i + 1];
    }

    // Set the last position to nullptr and decrement count
    activeEvents[--numEvents] = nullptr;
}

// Get an event at a specific index
Event* Kingdom::getEvent(int index) const {
    if (index < 0 || index >= numEvents) {
        throw out_of_range("Event index out of range");
    }
    return activeEvents[index];
}

// Get the number of active events
int Kingdom::getNumEvents() const {
    return numEvents;
}

// Process a turn for the kingdom
void Kingdom::processTurn() {
    // Start of turn message
    cout << "\n=== Turn " << turn << " - " << name << " ===\n" << endl;

    // Check if we have a leader
    if (!currentLeader) {
        cout << "Warning: The kingdom has no leader!" << endl;
        if (leadershipSystem && leadershipSystem->getNumPotentialLeaders() > 0) {
            leadershipSystem->handleSuccession();
        }
        else {
            cout << "There are no potential leaders. The kingdom is in anarchy!" << endl;
            stabilityLevel = max(0, stabilityLevel - 10);
        }
    }

    // Update leadership system
    if (leadershipSystem) {
        leadershipSystem->update();

        // Check for election
        if (leadershipSystem->getTurnsToNextElection() == 0) {
            leadershipSystem->holdElection();
        }

        // Check for coup
        if (leadershipSystem->checkForCoup()) {
            leadershipSystem->handleCoup();
        }
    }

    // Update weather
    if (currentWeather) {
        currentWeather->update();

        // Apply weather effects
        currentWeather->applyEffects(this);

        // Check if weather has ended
        if (!currentWeather->getIsActive()) {
            // Generate new weather
            Weather* newWeather = new Weather("Clear Skies", "The weather is fair.", 1, SUNNY);
            newWeather->generateRandomWeather();

            setCurrentWeather(newWeather);
        }
    }

    // Update disease if present
    if (currentDisease && currentDisease->getIsActive()) {
        currentDisease->update();

        // Apply disease effects
        currentDisease->applyEffects(this);
    }

    // Handle economy and resources
    if (economy) {
        // Get food resource for population consumption
        Resource* food = economy->getResourceByType(FOOD);
        int availableFood = food ? food->getAmount() : 0;

        // Update population with available food
        if (population) {
            population->update(availableFood);

            // Consume food based on population
            if (food) {
                int requiredFood = population->getTotalPopulation() * population->getFoodConsumptionPerCapita();
                food->consume(min(requiredFood, food->getAmount()));
            }
        }

        // Update economy
        economy->update();
    }

    // Update army
    if (army) {
        army->update();
    }

    // Update leader
    if (currentLeader) {
        currentLeader->update();
    }

    // Random events
    if (randomInt(1, 5) == 1) {
        Event* randomEvent = Event::generateRandomEvent();
        addEvent(randomEvent);
    }

    // Update active events
    for (int i = 0; i < numEvents; i++) {
        if (activeEvents[i]) {
            activeEvents[i]->update();

            // Remove expired events
            if (activeEvents[i]->getTurnsRemaining() <= 0) {
                cout << "The " << activeEvents[i]->getName() << " event has ended." << endl;
                removeEvent(i);
                i--; // Adjust index after removal
            }
        }
    }

    // Update bank
    if (bank) {
        bank->update();
    }

    // Calculate stability effects
    calculateStability();

    // Increment turn counter
    incrementTurn();

    // End of turn message
    cout << "\n=== End of Turn " << turn - 1 << " ===\n" << endl;
}

// Calculate kingdom stability based on various factors
void Kingdom::calculateStability() {
    int baseStability = stabilityLevel;

    // Population unrest reduces stability
    if (population->isUnrestActive()) {
        baseStability -= 10;
    }

    // Leader effects
    if (currentLeader) {
        // Higher leadership score increases stability
        baseStability += (currentLeader->getLeadershipScore() - 50) / 10;

        // Corruption reduces stability
        baseStability -= currentLeader->getCorruption() / 10;
    }
    else {
        // No leader is very destabilizing
        baseStability -= 15;
    }

    // Economy effects
    if (economy) {
        // Negative treasury is bad for stability
        Treasury* treasury = economy->getTreasury();
        if (treasury && treasury->getGold() < 0) {
            baseStability -= min(15, abs(treasury->getGold()) / 100);
        }

        // Market stability directly affects kingdom stability
        baseStability += (economy->getMarketStability() - 50) / 10;

        // High corruption reduces stability
        baseStability -= economy->getCorruptionLevel() / 10;
    }

    // Army effects
    if (army) {
        // Strong, high-morale army increases stability
        if (army->getTotalStrength() > 500 && army->getOverallMorale() > 70) {
            baseStability += 5;
        }
    }

    // Environmental effects
    if (currentWeather && currentWeather->getIsExtreme()) {
        baseStability -= 3;
    }

    if (currentDisease && currentDisease->getIsActive()) {
        baseStability -= currentDisease->getSeverity();
    }

    // Random factor
    baseStability += randomInt(-3, 3);

    // Constrain stability to valid range
    stabilityLevel = max(0, min(100, baseStability));

    // Report major stability changes
    if (stabilityLevel <= 20 && baseStability > 20) {
        cout << "WARNING: The kingdom's stability has fallen to dangerous levels!" << endl;
    }
    else if (stabilityLevel >= 80 && baseStability < 80) {
        cout << "The kingdom has achieved remarkable stability!" << endl;
    }
}

// Save kingdom data to file
void Kingdom::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save Kingdom: File not open");
    }

    file << name << endl;
    file << isPlayerControlled << endl;
    file << turn << endl;
    file << stabilityLevel << endl;

    // Save components
    population->save(file);
    economy->save(file);
    army->save(file);

    // Save current leader
    bool hasLeader = (currentLeader != nullptr);
    file << hasLeader << endl;
    if (hasLeader) {
        currentLeader->save(file);
    }

    // Save weather
    currentWeather->save(file);

    // Save disease
    bool hasDisease = (currentDisease != nullptr);
    file << hasDisease << endl;
    if (hasDisease) {
        currentDisease->save(file);
    }

    // Save leadership system
    leadershipSystem->save(file);

    // Save bank
    bank->save(file);

    // Save events
    file << numEvents << endl;
    for (int i = 0; i < numEvents; i++) {
        activeEvents[i]->save(file);
    }
}

// Load kingdom data from file
void Kingdom::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load Kingdom: File not open");
    }

    getline(file, name);
    file >> isPlayerControlled;
    file >> turn;
    file >> stabilityLevel;
    file.ignore(); // Skip newline

    // Load components
    population->load(file);
    economy->load(file);
    army->load(file);

    // Load current leader
    bool hasLeader;
    file >> hasLeader;
    file.ignore(); // Skip newline

    if (hasLeader) {
        if (currentLeader) {
            currentLeader->load(file);
        }
        else {
            currentLeader = new Leader("", "");
            currentLeader->load(file);
        }
    }
    else if (currentLeader) {
        delete currentLeader;
        currentLeader = nullptr;
    }

    // Load weather
    currentWeather->load(file);

    // Load disease
    bool hasDisease;
    file >> hasDisease;
    file.ignore(); // Skip newline

    if (hasDisease) {
        if (currentDisease) {
            currentDisease->load(file);
        }
        else {
            currentDisease = new Disease("", "", 1, 10, 5);
            currentDisease->load(file);
        }
    }
    else if (currentDisease) {
        delete currentDisease;
        currentDisease = nullptr;
    }

    // Load leadership system
    leadershipSystem->load(file);

    // Load bank
    bank->load(file);

    // Load events
    int loadedNumEvents;
    file >> loadedNumEvents;
    file.ignore(); // Skip newline

    // Clear existing events
    for (int i = 0; i < numEvents; i++) {
        delete activeEvents[i];
        activeEvents[i] = nullptr;
    }
    numEvents = 0;

    // Resize array if needed
    if (loadedNumEvents > maxEvents) {
        delete[] activeEvents;
        maxEvents = loadedNumEvents;
        activeEvents = new Event * [maxEvents];

        for (int i = 0; i < maxEvents; i++) {
            activeEvents[i] = nullptr;
        }
    }

    // Load each event
    for (int i = 0; i < loadedNumEvents; i++) {
        Event* newEvent = new Event("", "");
        newEvent->load(file);
        addEvent(newEvent);
    }
}