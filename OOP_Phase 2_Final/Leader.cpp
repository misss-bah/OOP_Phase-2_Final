#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Leader::Leader(const string& name, const string& title)
    : Human(name), title(title), intelligence(50), militarySkill(50), economicSkill(50),
    corruption(0), leadershipScore(0), experience(0), termLength(0), isElected(false) {

    // Initialize dynamic array for traits
    maxTraits = 5;
    numTraits = 0;
    traits = new LeadershipTrait * [maxTraits];

    for (int i = 0; i < maxTraits; i++) {
        traits[i] = nullptr;
    }

    // Calculate initial leadership score
    calculateLeadershipScore();
}

// Destructor
Leader::~Leader() {
    // Clean up dynamically allocated traits
    for (int i = 0; i < numTraits; i++) {
        delete traits[i];
    }

    // Clean up the array itself
    delete[] traits;
}

// Calculate leadership score based on attributes
void Leader::calculateLeadershipScore() {
    // Leadership score is a weighted sum of attributes
    leadershipScore = intelligence * 0.3 + militarySkill * 0.2 + economicSkill * 0.3 +
        (100 - corruption) * 0.2;

    // Experience bonus
    leadershipScore += experience / 10;

    // Trait modifications
    for (int i = 0; i < numTraits; i++) {
        if (traits[i]) {
            // Positive traits add to score, negative traits subtract
            if (traits[i]->getIsPositive()) {
                leadershipScore += 5;
            }
            else {
                leadershipScore -= 5;
            }
        }
    }
}

// Getters and setters
string Leader::getTitle() const {
    return title;
}

void Leader::setTitle(const string& newTitle) {
    title = newTitle;
}

int Leader::getIntelligence() const {
    return intelligence;
}

void Leader::setIntelligence(int value) {
    if (value < 0) value = 0;
    if (value > 100) value = 100;
    intelligence = value;
    calculateLeadershipScore();
}

int Leader::getMilitarySkill() const {
    return militarySkill;
}

void Leader::setMilitarySkill(int value) {
    if (value < 0) value = 0;
    if (value > 100) value = 100;
    militarySkill = value;
    calculateLeadershipScore();
}

int Leader::getEconomicSkill() const {
    return economicSkill;
}

void Leader::setEconomicSkill(int value) {
    if (value < 0) value = 0;
    if (value > 100) value = 100;
    economicSkill = value;
    calculateLeadershipScore();
}

int Leader::getCorruption() const {
    return corruption;
}

void Leader::setCorruption(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    corruption = level;
    calculateLeadershipScore();
}

int Leader::getLeadershipScore() const {
    return leadershipScore;
}

int Leader::getExperience() const {
    return experience;
}

void Leader::incrementExperience() {
    experience += randomInt(1, 3);
    if (experience > 100) experience = 100;
    calculateLeadershipScore();
}

int Leader::getTermLength() const {
    return termLength;
}

void Leader::incrementTermLength() {
    termLength++;
}

bool Leader::getIsElected() const {
    return isElected;
}

void Leader::setIsElected(bool elected) {
    isElected = elected;
}

// Add trait to the leader
void Leader::addTrait(LeadershipTrait* trait) {
    if (!trait) {
        throw invalid_argument("Cannot add null trait");
    }

    // Check if we need to resize the array
    if (numTraits >= maxTraits) {
        // Create a new, larger array
        int newMaxTraits = maxTraits * 2;
        LeadershipTrait** newTraits = new LeadershipTrait * [newMaxTraits];

        // Copy existing traits to the new array
        for (int i = 0; i < numTraits; i++) {
            newTraits[i] = traits[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numTraits; i < newMaxTraits; i++) {
            newTraits[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] traits;
        traits = newTraits;
        maxTraits = newMaxTraits;
    }

    // Add the new trait
    traits[numTraits++] = trait;

    // Apply trait effects
    trait->applyEffects(this);

    // Recalculate leadership score
    calculateLeadershipScore();
}

// Remove trait at a specific index
void Leader::removeTrait(int index) {
    if (index < 0 || index >= numTraits) {
        throw out_of_range("Trait index out of range");
    }

    // Remove trait effects first
    traits[index]->removeEffects(this);

    // Delete the trait object
    delete traits[index];

    // Shift remaining elements
    for (int i = index; i < numTraits - 1; i++) {
        traits[i] = traits[i + 1];
    }

    // Set the last position to nullptr and decrement count
    traits[--numTraits] = nullptr;

    // Recalculate leadership score
    calculateLeadershipScore();
}

// Get trait at a specific index
LeadershipTrait* Leader::getTrait(int index) const {
    if (index < 0 || index >= numTraits) {
        throw out_of_range("Trait index out of range");
    }
    return traits[index];
}

// Get number of traits
int Leader::getNumTraits() const {
    return numTraits;
}

// Calculate population bonus based on leader attributes
double Leader::calculatePopulationBonus() const {
    // Charisma and intelligence affect population happiness and loyalty
    double bonus = 1.0 + (happiness / 100.0) * 0.2;

    // Experience adds a small bonus
    bonus += experience / 500.0;

    // Corruption reduces the bonus
    bonus -= corruption / 200.0;

    return max(0.5, min(1.5, bonus));
}

// Calculate military bonus based on leader attributes
double Leader::calculateMilitaryBonus() const {
    // Military skill directly affects military effectiveness
    double bonus = 1.0 + (militarySkill / 100.0) * 0.5;

    // Experience adds a small bonus
    bonus += experience / 200.0;

    return max(0.7, min(2.0, bonus));
}

// Calculate economic bonus based on leader attributes
double Leader::calculateEconomicBonus() const {
    // Economic skill and intelligence affect economic performance
    double bonus = 1.0 + (economicSkill / 100.0) * 0.3 + (intelligence / 100.0) * 0.2;

    // Corruption reduces economic efficiency
    bonus -= corruption / 100.0;

    return max(0.5, min(1.8, bonus));
}

// Calculate disease prevention bonus based on leader attributes
double Leader::calculateDiseasePrevention() const {
    // Intelligence primarily affects disease prevention
    double bonus = 1.0 + (intelligence / 100.0) * 0.3;

    // Experience adds a small bonus
    bonus += experience / 300.0;

    // Corruption reduces prevention effectiveness
    bonus -= corruption / 200.0;

    return max(0.7, min(1.5, bonus));
}

// Calculate weather preparedness bonus based on leader attributes
double Leader::calculateWeatherPreparedness() const {
    // Intelligence and experience affect weather preparedness
    double bonus = 1.0 + (intelligence / 100.0) * 0.2 + (experience / 100.0) * 0.2;

    // Corruption reduces preparedness
    bonus -= corruption / 200.0;

    return max(0.6, min(1.6, bonus));
}

// Simulate leader making a decision based on difficulty
bool Leader::makeDecision(int difficulty) {
    if (difficulty < 1) difficulty = 1;
    if (difficulty > 10) difficulty = 10;

    // Calculate success threshold
    int threshold = 50 + difficulty * 5;

    // Intelligence and experience improve decision-making
    int decisionSkill = intelligence + experience / 2;

    // Corruption impairs decision-making
    decisionSkill -= corruption / 2;

    // Random factor
    decisionSkill += randomInt(-10, 10);

    // Decision is successful if skill exceeds threshold
    bool success = (decisionSkill >= threshold);

    // Experience gained from making decisions
    incrementExperience();

    return success;
}

// Update method required by Entity base class
void Leader::update() {
    // Call base class update
    Human::update();

    // Increment term length
    incrementTermLength();

    // Small chance for corruption to increase on its own
    if (randomInt(1, 20) == 1) {
        setCorruption(min(100, corruption + randomInt(1, 5)));
    }

    // Small chance for attributes to improve with experience
    if (randomInt(1, 10) == 1) {
        incrementExperience();

        // Random attribute improvement
        switch (randomInt(0, 2)) {
        case 0:
            setIntelligence(min(100, intelligence + 1));
            break;
        case 1:
            setMilitarySkill(min(100, militarySkill + 1));
            break;
        case 2:
            setEconomicSkill(min(100, economicSkill + 1));
            break;
        }
    }

    // Update all traits
    for (int i = 0; i < numTraits; i++) {
        if (traits[i]) {
            traits[i]->update();
        }
    }
}

// Save leader data to file
void Leader::save(ofstream& file) const {
    // Call base class save first
    Human::save(file);

    // Save Leader-specific attributes
    file << title << endl;
    file << intelligence << endl;
    file << militarySkill << endl;
    file << economicSkill << endl;
    file << corruption << endl;
    file << leadershipScore << endl;
    file << experience << endl;
    file << termLength << endl;
    file << isElected << endl;

    // Save traits
    file << numTraits << endl;
    for (int i = 0; i < numTraits; i++) {
        if (traits[i]) {
            traits[i]->save(file);
        }
    }
}

// Load leader data from file
void Leader::load(ifstream& file) {
    // Call base class load first
    Human::load(file);

    // Load Leader-specific attributes
    getline(file, title);
    file >> intelligence;
    file >> militarySkill;
    file >> economicSkill;
    file >> corruption;
    file >> leadershipScore;
    file >> experience;
    file >> termLength;
    file >> isElected;

    // Load traits
    int loadedNumTraits;
    file >> loadedNumTraits;
    file.ignore(); // Skip newline

    // Delete existing traits
    for (int i = 0; i < numTraits; i++) {
        delete traits[i];
        traits[i] = nullptr;
    }

    numTraits = 0;

    // Resize array if needed
    if (loadedNumTraits > maxTraits) {
        delete[] traits;
        maxTraits = loadedNumTraits;
        traits = new LeadershipTrait * [maxTraits];

        for (int i = 0; i < maxTraits; i++) {
            traits[i] = nullptr;
        }
    }

    // Load each trait
    for (int i = 0; i < loadedNumTraits; i++) {
        LeadershipTrait* newTrait = new LeadershipTrait("", "", true);
        newTrait->load(file);
        addTrait(newTrait);
    }
}
