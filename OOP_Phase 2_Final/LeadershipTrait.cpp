#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
LeadershipTrait::LeadershipTrait(const string& name, const string& description, bool isPositive, LeadershipTraitType type)
    : Entity(name, description), traitType(type), charismaEffect(0), intelligenceEffect(0),
    militaryEffect(0), economicEffect(0), corruptionEffect(0), isPositive(isPositive) {

    // Initialize effects based on trait type
    switch (type) {
    case INSPIRING:
        charismaEffect = 10;
        militaryEffect = 5;
        corruptionEffect = -5;
        break;

    case STRATEGIC:
        intelligenceEffect = 5;
        militaryEffect = 15;
        break;

    case RUTHLESS:
        militaryEffect = 15;
        corruptionEffect = 10;
        charismaEffect = -10;
        break;

    case DIPLOMATIC:
        charismaEffect = 15;
        economicEffect = 10;
        break;

    case CORRUPT:
        economicEffect = -5;
        corruptionEffect = 20;
        break;

    case CHARISMATIC:
        charismaEffect = 20;
        corruptionEffect = -5;
        break;

    case EXPERIENCED:
        intelligenceEffect = 10;
        militaryEffect = 5;
        economicEffect = 5;
        break;
    }

    // Invert effects for negative traits
    if (!isPositive) {
        charismaEffect = -charismaEffect;
        intelligenceEffect = -intelligenceEffect;
        militaryEffect = -militaryEffect;
        economicEffect = -economicEffect;
        corruptionEffect = -corruptionEffect;
    }
}

// Destructor
LeadershipTrait::~LeadershipTrait() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
LeadershipTraitType LeadershipTrait::getTraitType() const {
    return traitType;
}

void LeadershipTrait::setTraitType(LeadershipTraitType type) {
    traitType = type;
}

int LeadershipTrait::getCharismaEffect() const {
    return charismaEffect;
}

void LeadershipTrait::setCharismaEffect(int effect) {
    charismaEffect = effect;
}

int LeadershipTrait::getIntelligenceEffect() const {
    return intelligenceEffect;
}

void LeadershipTrait::setIntelligenceEffect(int effect) {
    intelligenceEffect = effect;
}

int LeadershipTrait::getMilitaryEffect() const {
    return militaryEffect;
}

void LeadershipTrait::setMilitaryEffect(int effect) {
    militaryEffect = effect;
}

int LeadershipTrait::getEconomicEffect() const {
    return economicEffect;
}

void LeadershipTrait::setEconomicEffect(int effect) {
    economicEffect = effect;
}

int LeadershipTrait::getCorruptionEffect() const {
    return corruptionEffect;
}

void LeadershipTrait::setCorruptionEffect(int effect) {
    corruptionEffect = effect;
}

bool LeadershipTrait::getIsPositive() const {
    return isPositive;
}

// Apply trait effects to a leader
void LeadershipTrait::applyEffects(Leader* leader) {
    if (!leader) {
        throw invalid_argument("Cannot apply trait effects: Leader is null");
    }

    // Apply intelligence effect
    if (intelligenceEffect != 0) {
        leader->setIntelligence(max(0, min(100, leader->getIntelligence() + intelligenceEffect)));
    }

    // Apply military effect
    if (militaryEffect != 0) {
        leader->setMilitarySkill(max(0, min(100, leader->getMilitarySkill() + militaryEffect)));
    }

    // Apply economic effect
    if (economicEffect != 0) {
        leader->setEconomicSkill(max(0, min(100, leader->getEconomicSkill() + economicEffect)));
    }

    // Apply corruption effect
    if (corruptionEffect != 0) {
        leader->setCorruption(max(0, min(100, leader->getCorruption() + corruptionEffect)));
    }

    // Apply charisma effect (affects happiness and loyalty)
    if (charismaEffect != 0) {
        leader->adjustHappiness(charismaEffect / 2);
        leader->adjustLoyalty(charismaEffect / 2);
    }
}

// Remove trait effects from a leader
void LeadershipTrait::removeEffects(Leader* leader) {
    if (!leader) {
        throw invalid_argument("Cannot remove trait effects: Leader is null");
    }

    // Remove effects by applying the opposite
    if (intelligenceEffect != 0) {
        leader->setIntelligence(max(0, min(100, leader->getIntelligence() - intelligenceEffect)));
    }

    if (militaryEffect != 0) {
        leader->setMilitarySkill(max(0, min(100, leader->getMilitarySkill() - militaryEffect)));
    }

    if (economicEffect != 0) {
        leader->setEconomicSkill(max(0, min(100, leader->getEconomicSkill() - economicEffect)));
    }

    if (corruptionEffect != 0) {
        leader->setCorruption(max(0, min(100, leader->getCorruption() - corruptionEffect)));
    }

    if (charismaEffect != 0) {
        leader->adjustHappiness(-(charismaEffect / 2));
        leader->adjustLoyalty(-(charismaEffect / 2));
    }
}

// Update method required by Entity base class
void LeadershipTrait::update() {
    // Traits generally don't change over time
    // But we could implement trait evolution or expiration here
    if (!isActive) return;

    // Very small chance for a trait to evolve or strengthen
    if (randomInt(1, 50) == 1) {
        if (isPositive) {
            // Positive traits might strengthen slightly
            charismaEffect += (charismaEffect > 0) ? 1 : 0;
            intelligenceEffect += (intelligenceEffect > 0) ? 1 : 0;
            militaryEffect += (militaryEffect > 0) ? 1 : 0;
            economicEffect += (economicEffect > 0) ? 1 : 0;
            corruptionEffect += (corruptionEffect < 0) ? -1 : 0; // Negative corruption is good
        }
        else {
            // Negative traits might weaken slightly
            charismaEffect += (charismaEffect < 0) ? 1 : 0;
            intelligenceEffect += (intelligenceEffect < 0) ? 1 : 0;
            militaryEffect += (militaryEffect < 0) ? 1 : 0;
            economicEffect += (economicEffect < 0) ? 1 : 0;
            corruptionEffect += (corruptionEffect > 0) ? -1 : 0;
        }
    }
}

// Save trait data to file
void LeadershipTrait::save(ofstream& file) const {
    // Call base class save first
    Entity::save(file);

    // Save LeadershipTrait-specific attributes
    file << static_cast<int>(traitType) << endl;
    file << charismaEffect << endl;
    file << intelligenceEffect << endl;
    file << militaryEffect << endl;
    file << economicEffect << endl;
    file << corruptionEffect << endl;
    file << isPositive << endl;
}

// Load trait data from file
void LeadershipTrait::load(ifstream& file) {
    // Call base class load first
    Entity::load(file);

    // Load LeadershipTrait-specific attributes
    int typeInt;
    file >> typeInt;
    traitType = static_cast<LeadershipTraitType>(typeInt);
    file >> charismaEffect;
    file >> intelligenceEffect;
    file >> militaryEffect;
    file >> economicEffect;
    file >> corruptionEffect;
    file >> isPositive;
    file.ignore(); // Skip newline
}
