#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Event::Event(const string& name, const string& description, EventType type)
    : Entity(name, description), type(type), populationEffect(0), economyEffect(0),
    militaryEffect(0), resourceEffect(0), duration(1), turnsRemaining(1),
    affectedResourceType(FOOD) {
}

// Destructor
Event::~Event() {
    // No dynamic memory to clean up in this class
}

// Getters and setters
EventType Event::getType() const {
    return type;
}

void Event::setType(EventType newType) {
    type = newType;
}

int Event::getPopulationEffect() const {
    return populationEffect;
}

void Event::setPopulationEffect(int effect) {
    populationEffect = effect;
}

int Event::getEconomyEffect() const {
    return economyEffect;
}

void Event::setEconomyEffect(int effect) {
    economyEffect = effect;
}

int Event::getMilitaryEffect() const {
    return militaryEffect;
}

void Event::setMilitaryEffect(int effect) {
    militaryEffect = effect;
}

int Event::getResourceEffect() const {
    return resourceEffect;
}

void Event::setResourceEffect(int effect) {
    resourceEffect = effect;
}

int Event::getDuration() const {
    return duration;
}

void Event::setDuration(int newDuration) {
    if (newDuration < 1) {
        throw invalid_argument("Duration must be at least 1");
    }
    duration = newDuration;
    turnsRemaining = newDuration;
}

int Event::getTurnsRemaining() const {
    return turnsRemaining;
}

void Event::decrementTurnsRemaining() {
    if (turnsRemaining > 0) {
        turnsRemaining--;
    }
}

ResourceType Event::getAffectedResourceType() const {
    return affectedResourceType;
}

void Event::setAffectedResourceType(ResourceType type) {
    affectedResourceType = type;
}

// Generate a random event
Event* Event::generateRandomEvent() {
    // List of potential event names and descriptions
    string eventNames[][3] = {
        {"Bountiful Harvest", "Drought", "Average Harvest"},
        {"Trade Boom", "Economic Crisis", "Market Fluctuations"},
        {"Military Parade", "Military Desertion", "New Recruits"},
        {"Gold Mine Discovery", "Resource Shortage", "Resource Discovery"},
        {"Population Growth", "Plague Outbreak", "Migration"}
    };

    string eventDescs[][3] = {
        {"A bountiful harvest has increased food supplies.", "A severe drought has damaged crops.", "The harvest is neither good nor bad this year."},
        {"Trade is booming in your kingdom.", "An economic crisis has hit your kingdom.", "Market prices are fluctuating."},
        {"A successful military parade has boosted morale.", "Soldiers are deserting in large numbers.", "New recruits have joined the army."},
        {"A new gold mine has been discovered.", "Resources are becoming scarce.", "A new resource deposit has been found."},
        {"The population is growing rapidly.", "A plague has broken out in your kingdom.", "People are migrating to and from your kingdom."}
    };

    // Choose random event type
    int eventCategory = randomInt(0, 4); // 5 categories of events
    EventType eventType;
    int typeIndex;

    // Determine if positive, negative, or neutral event (weighted)
    int random = randomInt(1, 10);
    if (random <= 4) { // 40% positive
        eventType = EVENT_POSITIVE;
        typeIndex = 0;
    }
    else if (random <= 8) { // 40% negative
        eventType = EVENT_NEGATIVE;
        typeIndex = 1;
    }
    else { // 20% neutral
        eventType = EVENT_NEUTRAL;
        typeIndex = 2;
    }

    // Create the event
    Event* event = new Event(eventNames[eventCategory][typeIndex], eventDescs[eventCategory][typeIndex], eventType);

    // Set duration (1-5 turns)
    event->setDuration(randomInt(1, 5));

    // Set effects based on type
    int effectMagnitude = randomInt(5, 20);

    switch (eventCategory) {
    case 0: // Harvest/Food event
        event->setAffectedResourceType(FOOD);
        event->setResourceEffect(eventType == EVENT_POSITIVE ? effectMagnitude : (eventType == EVENT_NEGATIVE ? -effectMagnitude : randomInt(-5, 5)));
        break;

    case 1: // Economic event
        event->setEconomyEffect(eventType == EVENT_POSITIVE ? effectMagnitude : (eventType == EVENT_NEGATIVE ? -effectMagnitude : randomInt(-5, 5)));
        event->setAffectedResourceType(GOLD);
        break;

    case 2: // Military event
        event->setMilitaryEffect(eventType == EVENT_POSITIVE ? effectMagnitude : (eventType == EVENT_NEGATIVE ? -effectMagnitude : randomInt(-5, 5)));
        break;

    case 3: // Resource event
        // Choose a random resource type
        event->setAffectedResourceType(static_cast<ResourceType>(randomInt(0, 4)));
        event->setResourceEffect(eventType == EVENT_POSITIVE ? effectMagnitude : (eventType == EVENT_NEGATIVE ? -effectMagnitude : randomInt(-5, 5)));
        break;

    case 4: // Population event
        event->setPopulationEffect(eventType == EVENT_POSITIVE ? effectMagnitude : (eventType == EVENT_NEGATIVE ? -effectMagnitude : randomInt(-5, 5)));
        break;
    }

    return event;
}

// Apply event effects to a kingdom
void Event::applyEffects(Kingdom* kingdom) {
    if (!kingdom) {
        throw invalid_argument("Cannot apply event effects: Kingdom is null");
    }

    cout << "Event: " << name << " - " << description << endl;

    // Population effects
    if (populationEffect != 0) {
        Population* population = kingdom->getPopulation();
        if (population) {
            population->handleEvent(*this);

            cout << "Population impact: " << (populationEffect > 0 ? "+" : "") << populationEffect << "%" << endl;
        }
    }

    // Economy effects
    if (economyEffect != 0) {
        Economy* economy = kingdom->getEconomy();
        if (economy) {
            if (economyEffect > 0) {
                economy->setProductionLevel(economy->getProductionLevel() + economyEffect / 2);
                economy->setTradeLevel(economy->getTradeLevel() + economyEffect / 3);

                Treasury* treasury = economy->getTreasury();
                if (treasury) {
                    int bonus = economyEffect * 10;
                    treasury->earn(bonus);
                    cout << "Treasury gained " << bonus << " gold." << endl;
                }
            }
            else {
                economy->setProductionLevel(max(10, economy->getProductionLevel() + economyEffect / 2));
                economy->setTradeLevel(max(10, economy->getTradeLevel() + economyEffect / 3));

                Treasury* treasury = economy->getTreasury();
                if (treasury) {
                    int loss = -economyEffect * 10;
                    treasury->spend(loss);
                    cout << "Treasury lost " << loss << " gold." << endl;
                }
            }

            cout << "Economy impact: " << (economyEffect > 0 ? "+" : "") << economyEffect << "%" << endl;
        }
    }

    // Military effects
    if (militaryEffect != 0) {
        Army* army = kingdom->getArmy();
        if (army) {
            if (militaryEffect > 0) {
                army->setTrainingLevel(army->getTrainingLevel() + militaryEffect / 3);

                // Boost morale
                for (int i = 0; i < army->getNumUnits(); i++) {
                    MilitaryUnit* unit = army->getUnit(i);
                    if (unit) {
                        unit->adjustMorale(militaryEffect / 2);
                    }
                }

                cout << "Army morale and training improved." << endl;
            }
            else {
                army->setTrainingLevel(max(10, army->getTrainingLevel() + militaryEffect / 3));

                // Reduce morale
                for (int i = 0; i < army->getNumUnits(); i++) {
                    MilitaryUnit* unit = army->getUnit(i);
                    if (unit) {
                        unit->adjustMorale(militaryEffect / 2);
                    }
                }

                cout << "Army morale and training declined." << endl;
            }

            cout << "Military impact: " << (militaryEffect > 0 ? "+" : "") << militaryEffect << "%" << endl;
        }
    }

    // Resource effects
    if (resourceEffect != 0) {
        Economy* economy = kingdom->getEconomy();
        if (economy) {
            Resource* affectedResource = economy->getResourceByType(affectedResourceType);
            if (affectedResource) {
                int currentAmount = affectedResource->getAmount();
                int change = (currentAmount * resourceEffect) / 100;

                if (change > 0) {
                    affectedResource->setAmount(currentAmount + change);
                    cout << affectedResource->getName() << " increased by " << change << "." << endl;
                }
                else {
                    affectedResource->setAmount(max(0, currentAmount + change));
                    cout << affectedResource->getName() << " decreased by " << -change << "." << endl;
                }
            }

            cout << "Resource impact: " << (resourceEffect > 0 ? "+" : "") << resourceEffect << "% on ";

            switch (affectedResourceType) {
            case FOOD: cout << "Food"; break;
            case WOOD: cout << "Wood"; break;
            case STONE: cout << "Stone"; break;
            case GOLD: cout << "Gold"; break;
            case IRON: cout << "Iron"; break;
            }

            cout << endl;
        }
    }
}

// Update method required by Entity base class
void Event::update() {
    if (isActive) {
        decrementTurnsRemaining();

        if (turnsRemaining <= 0) {
            isActive = false;
        }
    }
}

// Save event data to file
void Event::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save Event: File not open");
    }

    // Call base class save first
    Entity::save(file);

    // Save Event-specific attributes
    file << static_cast<int>(type) << endl;
    file << populationEffect << endl;
    file << economyEffect << endl;
    file << militaryEffect << endl;
    file << resourceEffect << endl;
    file << duration << endl;
    file << turnsRemaining << endl;
    file << static_cast<int>(affectedResourceType) << endl;
}

// Load event data from file
void Event::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load Event: File not open");
    }

    // Call base class load first
    Entity::load(file);

    // Load Event-specific attributes
    int typeInt;
    file >> typeInt;
    type = static_cast<EventType>(typeInt);

    file >> populationEffect;
    file >> economyEffect;
    file >> militaryEffect;
    file >> resourceEffect;
    file >> duration;
    file >> turnsRemaining;

    int resourceTypeInt;
    file >> resourceTypeInt;
    affectedResourceType = static_cast<ResourceType>(resourceTypeInt);

    file.ignore(); // Skip newline
}