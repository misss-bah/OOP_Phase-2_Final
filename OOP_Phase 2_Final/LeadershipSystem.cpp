#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
LeadershipSystem::LeadershipSystem(Kingdom* kingdom)
    : kingdom(kingdom), electionCycle(5), turnsToNextElection(5),
    stabilityFactor(50), coupRisk(10) {

    // Initialize dynamic array for potential leaders
    maxLeaders = 5;
    numLeaders = 0;
    potentialLeaders = new Leader * [maxLeaders];

    for (int i = 0; i < maxLeaders; i++) {
        potentialLeaders[i] = nullptr;
    }

    // Add some initial potential leaders if kingdom exists
    if (kingdom) {
        // Generate 2-4 potential leaders
        int initialLeaders = randomInt(2, 4);
        for (int i = 0; i < initialLeaders; i++) {
            addPotentialLeader(generateRandomLeader());
        }
    }
}

// Destructor
LeadershipSystem::~LeadershipSystem() {
    // Clean up dynamically allocated leaders
    for (int i = 0; i < numLeaders; i++) {
        delete potentialLeaders[i];
    }

    // Clean up the array itself
    delete[] potentialLeaders;
}

// Add a potential leader
void LeadershipSystem::addPotentialLeader(Leader* leader) {
    if (!leader) {
        throw invalid_argument("Cannot add null leader");
    }

    // Check if we need to resize the array
    if (numLeaders >= maxLeaders) {
        // Create a new, larger array
        int newMaxLeaders = maxLeaders * 2;
        Leader** newLeaders = new Leader * [newMaxLeaders];

        // Copy existing leaders to the new array
        for (int i = 0; i < numLeaders; i++) {
            newLeaders[i] = potentialLeaders[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numLeaders; i < newMaxLeaders; i++) {
            newLeaders[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] potentialLeaders;
        potentialLeaders = newLeaders;
        maxLeaders = newMaxLeaders;
    }

    // Add the new leader
    potentialLeaders[numLeaders++] = leader;
}

// Remove a potential leader at a specific index
void LeadershipSystem::removePotentialLeader(int index) {
    if (index < 0 || index >= numLeaders) {
        throw out_of_range("Leader index out of range");
    }

    // If this is the current kingdom leader, don't delete it
    // as the Kingdom class has responsibility for the current leader
    if (kingdom && potentialLeaders[index] == kingdom->getCurrentLeader()) {
        // Just shift the pointers without deleting
        for (int i = index; i < numLeaders - 1; i++) {
            potentialLeaders[i] = potentialLeaders[i + 1];
        }
        potentialLeaders[--numLeaders] = nullptr;
        return;
    }

    // Otherwise, delete the leader object
    delete potentialLeaders[index];

    // Shift remaining elements
    for (int i = index; i < numLeaders - 1; i++) {
        potentialLeaders[i] = potentialLeaders[i + 1];
    }

    // Set the last position to nullptr and decrement count
    potentialLeaders[--numLeaders] = nullptr;
}

// Get a potential leader at a specific index
Leader* LeadershipSystem::getPotentialLeader(int index) const {
    if (index < 0 || index >= numLeaders) {
        throw out_of_range("Leader index out of range");
    }
    return potentialLeaders[index];
}

// Get number of potential leaders
int LeadershipSystem::getNumPotentialLeaders() const {
    return numLeaders;
}

// Getters and setters
int LeadershipSystem::getElectionCycle() const {
    return electionCycle;
}

void LeadershipSystem::setElectionCycle(int cycles) {
    if (cycles < 1) cycles = 1;
    electionCycle = cycles;
}

int LeadershipSystem::getTurnsToNextElection() const {
    return turnsToNextElection;
}

int LeadershipSystem::getStabilityFactor() const {
    return stabilityFactor;
}

void LeadershipSystem::setStabilityFactor(int stability) {
    if (stability < 0) stability = 0;
    if (stability > 100) stability = 100;
    stabilityFactor = stability;
}

int LeadershipSystem::getCoupRisk() const {
    return coupRisk;
}

void LeadershipSystem::setCoupRisk(int risk) {
    if (risk < 0) risk = 0;
    if (risk > 100) risk = 100;
    coupRisk = risk;
}

// Generate a random leader
Leader* LeadershipSystem::generateRandomLeader() {
    // List of potential titles
    string titles[] = { "Lord", "Duke", "Baron", "Count", "Knight", "General", "Chancellor" };

    // List of potential names (first and last)
    string firstNames[] = { "John", "Richard", "William", "Henry", "Robert", "Thomas", "Edward", "Charles" };
    string lastNames[] = { "Smith", "Young", "Wilson", "Cooper", "Miller", "Baker", "Fletcher", "Stewart" };

    // Generate random name and title
    string name = firstNames[randomInt(0, 7)] + " " + lastNames[randomInt(0, 7)];
    string title = titles[randomInt(0, 6)];

    // Create new leader
    Leader* leader = new Leader(name, title);

    // Randomize attributes (40-90 range to make them competent but varied)
    leader->setIntelligence(randomInt(40, 90));
    leader->setMilitarySkill(randomInt(40, 90));
    leader->setEconomicSkill(randomInt(40, 90));
    leader->setCorruption(randomInt(5, 30)); // Some initial corruption

    // Add 1-3 random traits
    int numTraits = randomInt(1, 3);

    for (int i = 0; i < numTraits; i++) {
        // Randomly select trait type
        LeadershipTraitType traitType = static_cast<LeadershipTraitType>(randomInt(0, 6));

        // 70% chance for a positive trait, 30% for negative
        bool isPositive = (randomInt(1, 10) <= 7);

        // Create trait names and descriptions based on type and positivity
        string traitName, traitDesc;

        switch (traitType) {
        case INSPIRING:
            traitName = isPositive ? "Inspiring" : "Uninspiring";
            traitDesc = isPositive ? "Boosts morale of troops and citizens" : "Fails to motivate others";
            break;
        case STRATEGIC:
            traitName = isPositive ? "Strategic Genius" : "Poor Tactician";
            traitDesc = isPositive ? "Skilled at planning and warfare" : "Makes poor strategic decisions";
            break;
        case RUTHLESS:
            traitName = isPositive ? "Determined" : "Cruel";
            traitDesc = isPositive ? "Willing to make tough decisions" : "Unnecessarily harsh to subjects";
            break;
        case DIPLOMATIC:
            traitName = isPositive ? "Diplomatic" : "Offensive";
            traitDesc = isPositive ? "Skilled at negotiations and diplomacy" : "Tends to insult and alienate others";
            break;
        case CORRUPT:
            traitName = isPositive ? "Honest" : "Corrupt";
            traitDesc = isPositive ? "Trustworthy and transparent" : "Takes bribes and embezzles funds";
            break;
        case CHARISMATIC:
            traitName = isPositive ? "Charismatic" : "Dull";
            traitDesc = isPositive ? "Charming and likable" : "Boring and forgettable";
            break;
        case EXPERIENCED:
            traitName = isPositive ? "Experienced" : "Inexperienced";
            traitDesc = isPositive ? "Seasoned in governance" : "New to leadership roles";
            break;
        }

        // Create and add the trait
        LeadershipTrait* trait = new LeadershipTrait(traitName, traitDesc, isPositive, traitType);
        leader->addTrait(trait);
    }

    return leader;
}

// Hold an election to determine the new leader
void LeadershipSystem::holdElection() {
    if (!kingdom) {
        throw runtime_error("Cannot hold election: Kingdom is null");
    }

    cout << "An election is being held in " << kingdom->getName() << "!" << endl;

    // Need at least 2 leaders for an election
    if (numLeaders < 2) {
        // Generate some candidates if necessary
        while (numLeaders < 3) {
            addPotentialLeader(generateRandomLeader());
        }
    }

    // Current leader (if elected) has an advantage
    Leader* currentLeader = kingdom->getCurrentLeader();
    bool incumbentAdvantage = false;

    if (currentLeader && currentLeader->getIsElected()) {
        incumbentAdvantage = true;
    }

    // Calculate scores for each candidate
    int* scores = new int[numLeaders];

    for (int i = 0; i < numLeaders; i++) {
        Leader* candidate = potentialLeaders[i];

        // Base score is leadership score
        scores[i] = candidate->getLeadershipScore();

        // Incumbent advantage
        if (incumbentAdvantage && candidate == currentLeader) {
            scores[i] += 20;
        }

        // Random factor (public opinion, campaign effectiveness)
        scores[i] += randomInt(-10, 20);

        // Corruption reduces score
        scores[i] -= candidate->getCorruption() / 2;

        cout << "Candidate: " << candidate->getTitle() << " " << candidate->getName()
            << " - Election Score: " << scores[i] << endl;
    }

    // Find the winner
    int winnerIndex = 0;
    for (int i = 1; i < numLeaders; i++) {
        if (scores[i] > scores[winnerIndex]) {
            winnerIndex = i;
        }
    }

    // Set the winner as the current leader
    Leader* winner = potentialLeaders[winnerIndex];
    winner->setIsElected(true);

    // If the winner is already the current leader, increment term
    if (winner == currentLeader) {
        cout << currentLeader->getTitle() << " " << currentLeader->getName()
            << " has been re-elected!" << endl;
    }
    else {
        // Set the new leader
        cout << winner->getTitle() << " " << winner->getName()
            << " has been elected as the new leader!" << endl;
        kingdom->setCurrentLeader(winner);
    }

    // Reset election timer
    turnsToNextElection = electionCycle;

    // Clean up
    delete[] scores;
}

// Check if a coup is likely to occur
bool LeadershipSystem::checkForCoup() {
    if (!kingdom) {
        return false;
    }

    Leader* currentLeader = kingdom->getCurrentLeader();
    if (!currentLeader) {
        return false;
    }

    // Factors that increase coup risk:
    // - High corruption
    // - Low stability
    // - Low leader charisma/loyalty
    // - Military dissatisfaction

    int baseCoupRisk = coupRisk;

    // Leader corruption increases coup risk
    baseCoupRisk += currentLeader->getCorruption() / 5;

    // Low stability increases coup risk
    baseCoupRisk += (100 - stabilityFactor) / 5;

    // Low leader loyalty/happiness decreases coup risk
    baseCoupRisk -= currentLeader->getLoyaltyLevel() / 5;

    // Military factors
    Army* army = kingdom->getArmy();
    if (army) {
        // Low morale increases coup risk
        baseCoupRisk += (70 - army->getOverallMorale()) / 3;
    }

    // Final coup risk (capped)
    int finalCoupRisk = max(5, min(95, baseCoupRisk));

    // Random check for coup
    return (randomInt(1, 100) <= finalCoupRisk);
}

// Handle a coup against the current leader
void LeadershipSystem::handleCoup() {
    if (!kingdom) {
        throw runtime_error("Cannot handle coup: Kingdom is null");
    }

    Leader* currentLeader = kingdom->getCurrentLeader();
    if (!currentLeader) {
        throw runtime_error("Cannot handle coup: Current leader is null");
    }

    cout << "A military coup has been launched against " << currentLeader->getTitle()
        << " " << currentLeader->getName() << "!" << endl;

    // Need at least one potential replacement
    if (numLeaders < 2) {
        // Generate a new leader if necessary
        Leader* newLeader = generateRandomLeader();
        addPotentialLeader(newLeader);
    }

    // Find the most militaristic leader as coup leader
    int coupLeaderIndex = -1;
    int highestMilitarySkill = -1;

    for (int i = 0; i < numLeaders; i++) {
        if (potentialLeaders[i] != currentLeader &&
            potentialLeaders[i]->getMilitarySkill() > highestMilitarySkill) {
            highestMilitarySkill = potentialLeaders[i]->getMilitarySkill();
            coupLeaderIndex = i;
        }
    }

    // If no suitable coup leader found, generate one
    if (coupLeaderIndex == -1) {
        Leader* militaryLeader = generateRandomLeader();
        militaryLeader->setMilitarySkill(randomInt(70, 90)); // Higher military skill
        militaryLeader->setTitle("General"); // Military title
        addPotentialLeader(militaryLeader);
        coupLeaderIndex = numLeaders - 1;
    }

    Leader* coupLeader = potentialLeaders[coupLeaderIndex];

    // Coup success probability based on military skill difference and stability
    int coupSuccess = 50; // Base chance
    coupSuccess += (coupLeader->getMilitarySkill() - currentLeader->getMilitarySkill());
    coupSuccess += (70 - stabilityFactor) / 2;

    // Loyalty of the current leader affects coup success
    coupSuccess -= currentLeader->getLoyaltyLevel() / 2;

    // Cap the success probability
    coupSuccess = max(10, min(90, coupSuccess));

    // Determine if coup succeeds
    if (randomInt(1, 100) <= coupSuccess) {
        cout << "The coup has succeeded! " << coupLeader->getTitle() << " "
            << coupLeader->getName() << " has seized power!" << endl;

        // Set the new leader
        kingdom->setCurrentLeader(coupLeader);
        coupLeader->setIsElected(false); // Not elected

        // Reduce stability
        stabilityFactor = max(10, stabilityFactor - 20);

        // Increase coup risk (coups tend to beget more coups)
        coupRisk = min(100, coupRisk + 15);
    }
    else {
        cout << "The coup has failed! " << currentLeader->getTitle() << " "
            << currentLeader->getName() << " remains in power." << endl;

        // Remove the coup leader
        removePotentialLeader(coupLeaderIndex);

        // Temporarily increase stability (rallying effect)
        stabilityFactor = min(100, stabilityFactor + 10);

        // Decrease coup risk temporarily
        coupRisk = max(5, coupRisk - 15);
    }
}

// Handle the death of a leader
void LeadershipSystem::handleDeath() {
    if (!kingdom) {
        throw runtime_error("Cannot handle death: Kingdom is null");
    }

    Leader* currentLeader = kingdom->getCurrentLeader();
    if (!currentLeader) {
        throw runtime_error("Cannot handle death: Current leader is null");
    }

    cout << currentLeader->getTitle() << " " << currentLeader->getName()
        << " has died!" << endl;

    // Remove the dead leader from potential leaders list
    for (int i = 0; i < numLeaders; i++) {
        if (potentialLeaders[i] == currentLeader) {
            // Just remove from the list, don't delete (Kingdom has ownership)
            for (int j = i; j < numLeaders - 1; j++) {
                potentialLeaders[j] = potentialLeaders[j + 1];
            }
            potentialLeaders[--numLeaders] = nullptr;
            break;
        }
    }

    // Handle succession
    handleSuccession();
}

// Handle succession after a leader's death or removal
void LeadershipSystem::handleSuccession() {
    if (!kingdom) {
        throw runtime_error("Cannot handle succession: Kingdom is null");
    }

    // Need at least one potential successor
    if (numLeaders < 1) {
        // Generate new potential leaders
        int newLeaders = randomInt(2, 4);
        for (int i = 0; i < newLeaders; i++) {
            addPotentialLeader(generateRandomLeader());
        }
    }

    // Determine succession method based on stability
    if (stabilityFactor >= 70 && randomInt(1, 100) <= 70) {
        // Stable condition - peaceful election
        holdElection();
    }
    else {
        // Unstable condition - most powerful takes over

        // Find the leader with the highest leadership score
        int successorIndex = 0;
        for (int i = 1; i < numLeaders; i++) {
            if (potentialLeaders[i]->getLeadershipScore() > potentialLeaders[successorIndex]->getLeadershipScore()) {
                successorIndex = i;
            }
        }

        Leader* successor = potentialLeaders[successorIndex];
        successor->setIsElected(false); // Not elected

        cout << successor->getTitle() << " " << successor->getName()
            << " has assumed leadership of the kingdom." << endl;

        // Set the new leader
        kingdom->setCurrentLeader(successor);

        // Succession crisis reduces stability
        stabilityFactor = max(10, stabilityFactor - 10);
    }
}

// Update the leadership system
void LeadershipSystem::update() {
    if (!kingdom) {
        return;
    }

    Leader* currentLeader = kingdom->getCurrentLeader();
    if (!currentLeader) {
        // No current leader, need succession
        handleSuccession();
        return;
    }

    // Update election timer
    if (turnsToNextElection > 0) {
        turnsToNextElection--;
    }

    // Check if it's election time
    if (turnsToNextElection <= 0) {
        holdElection();
    }

    // Update stability based on current conditions

    // Leader factors
    if (currentLeader->getCorruption() > 70) {
        stabilityFactor = max(10, stabilityFactor - 2);
    }
    else if (currentLeader->getCorruption() < 30) {
        stabilityFactor = min(100, stabilityFactor + 1);
    }

    if (currentLeader->getHappiness() < 30) {
        stabilityFactor = max(10, stabilityFactor - 1);
    }
    else if (currentLeader->getHappiness() > 70) {
        stabilityFactor = min(100, stabilityFactor + 1);
    }

    // Population factors
    Population* population = kingdom->getPopulation();
    if (population) {
        if (population->isUnrestActive()) {
            stabilityFactor = max(10, stabilityFactor - 5);
            coupRisk = min(100, coupRisk + 3);
        }
        else {
            stabilityFactor = min(100, stabilityFactor + 1);
        }
    }

    // Economy factors
    Economy* economy = kingdom->getEconomy();
    if (economy) {
        Treasury* treasury = economy->getTreasury();
        if (treasury && treasury->getGold() < 100) {
            stabilityFactor = max(10, stabilityFactor - 2);
            coupRisk = min(100, coupRisk + 2);
        }
    }

    // Chance for a random stability event
    if (randomInt(1, 20) == 1) {
        int change = randomInt(-5, 5);
        stabilityFactor = max(10, min(100, stabilityFactor + change));

        if (change > 0) {
            cout << "Political stability has improved in the kingdom." << endl;
        }
        else if (change < 0) {
            cout << "Political stability has deteriorated in the kingdom." << endl;
        }
    }

    // Check for coup
    if (checkForCoup()) {
        handleCoup();
    }

    // Small chance for leader death (higher for older terms)
    int deathChance = 1 + currentLeader->getTermLength() / 10;
    if (randomInt(1, 100) <= deathChance) {
        handleDeath();
    }

    // Randomly generate new potential leaders
    if (numLeaders < 3 && randomInt(1, 5) == 1) {
        addPotentialLeader(generateRandomLeader());
        cout << "A new potential leader, " << potentialLeaders[numLeaders - 1]->getName()
            << ", has emerged in the kingdom." << endl;
    }
}

// Save leadership system data to file
void LeadershipSystem::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save LeadershipSystem: File not open");
    }

    file << electionCycle << endl;
    file << turnsToNextElection << endl;
    file << stabilityFactor << endl;
    file << coupRisk << endl;

    // Save number of potential leaders
    file << numLeaders << endl;

    // Save each potential leader
    for (int i = 0; i < numLeaders; i++) {
        if (potentialLeaders[i]) {
            // Check if this is the current leader
            bool isCurrentLeader = (kingdom && potentialLeaders[i] == kingdom->getCurrentLeader());
            file << isCurrentLeader << endl;

            // Only save if not the current leader (to avoid duplication)
            if (!isCurrentLeader) {
                potentialLeaders[i]->save(file);
            }
        }
    }
}

// Load leadership system data from file
void LeadershipSystem::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load LeadershipSystem: File not open");
    }

    file >> electionCycle;
    file >> turnsToNextElection;
    file >> stabilityFactor;
    file >> coupRisk;

    // Load number of potential leaders
    int loadedNumLeaders;
    file >> loadedNumLeaders;
    file.ignore(); // Skip newline

    // Clean up existing leaders
    for (int i = 0; i < numLeaders; i++) {
        // Don't delete the current leader (kingdom owns it)
        if (kingdom && potentialLeaders[i] == kingdom->getCurrentLeader()) {
            continue;
        }
        delete potentialLeaders[i];
        potentialLeaders[i] = nullptr;
    }

    numLeaders = 0;

    // Resize array if needed
    if (loadedNumLeaders > maxLeaders) {
        delete[] potentialLeaders;
        maxLeaders = loadedNumLeaders;
        potentialLeaders = new Leader * [maxLeaders];

        for (int i = 0; i < maxLeaders; i++) {
            potentialLeaders[i] = nullptr;
        }
    }

    // Load each potential leader
    for (int i = 0; i < loadedNumLeaders; i++) {
        bool isCurrentLeader;
        file >> isCurrentLeader;
        file.ignore(); // Skip newline

        if (isCurrentLeader) {
            // Add current leader to the list without loading (Kingdom loads it)
            if (kingdom && kingdom->getCurrentLeader()) {
                addPotentialLeader(kingdom->getCurrentLeader());
            }
        }
        else {
            // Load a new leader
            Leader* newLeader = new Leader("", "");
            newLeader->load(file);
            addPotentialLeader(newLeader);
        }
    }
}
