#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
GameEngine::GameEngine()
    : isGameRunning(false), isGamePaused(false), gameSpeed(1),
    difficulty(NORMAL), currentTurn(0), maxKingdoms(5),
    isMultiplayerMode(false), numHumanPlayers(1), chatbotEnabled(false) {

    // Initialize kingdoms array
    kingdoms = new Kingdom * [maxKingdoms];
    numKingdoms = 0;

    for (int i = 0; i < maxKingdoms; i++) {
        kingdoms[i] = nullptr;
    }

    // No player kingdom initially
    playerKingdom = nullptr;
}

// Destructor
GameEngine::~GameEngine() {
    // Clean up all kingdoms
    for (int i = 0; i < numKingdoms; i++) {
        delete kingdoms[i];
    }

    // Delete the array itself
    delete[] kingdoms;
}

// Getters and setters
bool GameEngine::getIsGameRunning() const {
    return isGameRunning;
}

void GameEngine::setIsGameRunning(bool running) {
    isGameRunning = running;
}

bool GameEngine::getIsGamePaused() const {
    return isGamePaused;
}

void GameEngine::setIsGamePaused(bool paused) {
    isGamePaused = paused;
}

int GameEngine::getGameSpeed() const {
    return gameSpeed;
}

void GameEngine::setGameSpeed(int speed) {
    if (speed < 1) speed = 1;
    if (speed > 5) speed = 5;
    gameSpeed = speed;
}

GameDifficulty GameEngine::getDifficulty() const {
    return difficulty;
}

void GameEngine::setDifficulty(GameDifficulty diff) {
    difficulty = diff;
}

int GameEngine::getCurrentTurn() const {
    return currentTurn;
}

Kingdom* GameEngine::getPlayerKingdom() const {
    return playerKingdom;
}

void GameEngine::setPlayerKingdom(Kingdom* kingdom) {
    playerKingdom = kingdom;
}

// Add a kingdom to the game
void GameEngine::addKingdom(Kingdom* kingdom) {
    if (!kingdom) {
        throw invalid_argument("Cannot add null kingdom");
    }

    // Check if we need to resize the array
    if (numKingdoms >= maxKingdoms) {
        // Create a new, larger array
        int newMaxKingdoms = maxKingdoms * 2;
        Kingdom** newKingdoms = new Kingdom * [newMaxKingdoms];

        // Copy existing kingdoms to the new array
        for (int i = 0; i < numKingdoms; i++) {
            newKingdoms[i] = kingdoms[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numKingdoms; i < newMaxKingdoms; i++) {
            newKingdoms[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] kingdoms;
        kingdoms = newKingdoms;
        maxKingdoms = newMaxKingdoms;
    }

    // Add the new kingdom
    kingdoms[numKingdoms++] = kingdom;
}

// Remove a kingdom at a specific index
void GameEngine::removeKingdom(int index) {
    if (index < 0 || index >= numKingdoms) {
        throw out_of_range("Kingdom index out of range");
    }

    // If this is the player kingdom, update the pointer
    if (kingdoms[index] == playerKingdom) {
        playerKingdom = nullptr;
    }

    // Delete the kingdom object
    delete kingdoms[index];

    // Shift remaining elements
    for (int i = index; i < numKingdoms - 1; i++) {
        kingdoms[i] = kingdoms[i + 1];
    }

    // Set the last position to nullptr and decrement count
    kingdoms[--numKingdoms] = nullptr;
}

// Get a kingdom at a specific index
Kingdom* GameEngine::getKingdom(int index) const {
    if (index < 0 || index >= numKingdoms) {
        throw out_of_range("Kingdom index out of range");
    }
    return kingdoms[index];
}

// Get number of kingdoms
int GameEngine::getNumKingdoms() const {
    return numKingdoms;
}

// Start the game
void GameEngine::startGame() {
    if (numKingdoms == 0) {
        cout << "Cannot start game: No kingdoms present" << endl;
        return;
    }

    if (!playerKingdom) {
        cout << "Cannot start game: No player kingdom set" << endl;
        return;
    }

    isGameRunning = true;
    isGamePaused = false;
    currentTurn = 1;

    cout << "Starting the game..." << endl;

    // Initial game state display
    displayGameState();

    // Enter the main game loop
    while (isGameRunning) {
        processTurn();
    }
}

// Process a game turn
void GameEngine::processTurn() {
    if (!isGameRunning || isGamePaused) {
        return;
    }

    cout << "\n=== TURN " << currentTurn << " ===\n" << endl;

    // Process each kingdom's turn
    for (int i = 0; i < numKingdoms; i++) {
        if (kingdoms[i]) {
            kingdoms[i]->processTurn();
        }
    }

    // Handle AI decisions for non-player kingdoms
    handleAIDecisions();

    // Check for game ending conditions
    checkGameEndingConditions();

    // Increment turn counter
    currentTurn++;

    // Pause for player input between turns if it's the player's kingdom
    if (isGameRunning && playerKingdom) {
        userInterface();
    }
}

// Display current game state
void GameEngine::displayGameState() const {
    cout << "=== GAME STATE - TURN " << currentTurn << " ===\n" << endl;

    // Display player kingdom information
    if (playerKingdom) {
        cout << "Your Kingdom: " << playerKingdom->getName() << endl;

        Leader* leader = playerKingdom->getCurrentLeader();
        if (leader) {
            cout << "Leader: " << leader->getTitle() << " " << leader->getName() << endl;
            cout << "Leadership Score: " << leader->getLeadershipScore() << endl;
        }
        else {
            cout << "No current leader!" << endl;
        }

        Population* population = playerKingdom->getPopulation();
        if (population) {
            cout << "Population: " << population->getTotalPopulation() << endl;
            cout << "Health Level: " << population->getHealthLevel() << endl;
            cout << "Unrest: " << (population->isUnrestActive() ? "Active" : "None") << endl;
        }

        Economy* economy = playerKingdom->getEconomy();
        if (economy) {
            Treasury* treasury = economy->getTreasury();
            if (treasury) {
                cout << "Treasury: " << treasury->getGold() << " gold" << endl;
                cout << "Income: " << treasury->getIncome() << " gold per turn" << endl;
                cout << "Expenses: " << treasury->getExpenses() << " gold per turn" << endl;
            }

            cout << "Production Level: " << economy->getProductionLevel() << endl;
            cout << "Trade Level: " << economy->getTradeLevel() << endl;
        }

        Army* army = playerKingdom->getArmy();
        if (army) {
            cout << "Army Strength: " << army->getTotalStrength() << endl;
            cout << "Army Morale: " << army->getOverallMorale() << endl;
            cout << "Military Units: " << army->getNumUnits() << endl;
        }

        Weather* weather = playerKingdom->getCurrentWeather();
        if (weather) {
            cout << "Weather: " << weather->getName() << endl;
            cout << "Weather Severity: " << weather->getSeverity() << endl;
            cout << "Weather Effects: " << (weather->getIsExtreme() ? "Extreme" : "Normal") << endl;
        }

        Disease* disease = playerKingdom->getCurrentDisease();
        if (disease && disease->getIsActive()) {
            cout << "Disease: " << disease->getName() << endl;
            cout << "Infected: " << disease->getCurrentInfected() << endl;
            cout << "Severity: " << disease->getSeverity() << endl;
        }

        cout << "Kingdom Stability: " << playerKingdom->getStabilityLevel() << endl;
    }
    else {
        cout << "No player kingdom found!" << endl;
    }

    // Display information about other kingdoms
    cout << "\nOther Kingdoms:" << endl;
    for (int i = 0; i < numKingdoms; i++) {
        if (kingdoms[i] && kingdoms[i] != playerKingdom) {
            cout << "- " << kingdoms[i]->getName();

            Leader* leader = kingdoms[i]->getCurrentLeader();
            if (leader) {
                cout << " (led by " << leader->getTitle() << " " << leader->getName() << ")";
            }

            cout << endl;
        }
    }

    cout << endl;
}

// Handle user interface for player decisions
void GameEngine::userInterface() {
    cout << "\n=== KINGDOM MANAGEMENT ===\n" << endl;
    cout << "1. View Detailed Kingdom Information" << endl;
    cout << "2. Manage Economy" << endl;
    cout << "3. Manage Military" << endl;
    cout << "4. Manage Population" << endl;
    cout << "5. Diplomacy" << endl;
    cout << "6. Save Game" << endl;
    cout << "7. End Turn" << endl;
    cout << "8. Quit Game" << endl;

    int choice;
    cout << "Enter your choice: ";
    cin >> choice;

    // Clear input buffer
    cin.clear();
    cin.ignore(1000, '\n');

    switch (choice) {
    case 1:
        displayDetailedKingdomInfo();
        userInterface(); // Return to menu
        break;
    case 2:
        economyInterface();
        userInterface(); // Return to menu
        break;
    case 3:
        militaryInterface();
        userInterface(); // Return to menu
        break;
    case 4:
        populationInterface();
        userInterface(); // Return to menu
        break;
    case 5:
        diplomacyInterface();
        userInterface(); // Return to menu
        break;
    case 6:
        saveGame();
        userInterface(); // Return to menu
        break;
    case 7:
        // End turn (return to main loop)
        break;
    case 8:
        cout << "Are you sure you want to quit? (y/n): ";
        char confirm;
        cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') {
            isGameRunning = false;
        }
        else {
            userInterface(); // Return to menu
        }
        break;
    default:
        cout << "Invalid choice. Please try again." << endl;
        userInterface(); // Return to menu
        break;
    }
}

// Display detailed information about the player's kingdom
void GameEngine::displayDetailedKingdomInfo() const {
    if (!playerKingdom) {
        cout << "No player kingdom found!" << endl;
        return;
    }

    cout << "\n=== DETAILED KINGDOM INFORMATION ===\n" << endl;
    cout << "Kingdom: " << playerKingdom->getName() << endl;
    cout << "Turn: " << currentTurn << endl;
    cout << "Stability: " << playerKingdom->getStabilityLevel() << endl;

    // Leader information
    Leader* leader = playerKingdom->getCurrentLeader();
    if (leader) {
        cout << "\n--- LEADER ---" << endl;
        cout << "Name: " << leader->getTitle() << " " << leader->getName() << endl;
        cout << "Leadership Score: " << leader->getLeadershipScore() << endl;
        cout << "Intelligence: " << leader->getIntelligence() << endl;
        cout << "Military Skill: " << leader->getMilitarySkill() << endl;
        cout << "Economic Skill: " << leader->getEconomicSkill() << endl;
        cout << "Corruption: " << leader->getCorruption() << endl;
        cout << "Experience: " << leader->getExperience() << endl;
        cout << "Term Length: " << leader->getTermLength() << " turns" << endl;
        cout << "Elected: " << (leader->getIsElected() ? "Yes" : "No") << endl;

        cout << "Traits:" << endl;
        for (int i = 0; i < leader->getNumTraits(); i++) {
            LeadershipTrait* trait = leader->getTrait(i);
            if (trait) {
                cout << "- " << trait->getName() << ": " << trait->getDescription() << endl;
            }
        }
    }
    else {
        cout << "\n--- NO LEADER ---" << endl;
    }

    // Population information
    Population* population = playerKingdom->getPopulation();
    if (population) {
        cout << "\n--- POPULATION ---" << endl;
        cout << "Total Population: " << population->getTotalPopulation() << endl;
        cout << "Growth Rate: " << population->getGrowthRate() << endl;
        cout << "Health Level: " << population->getHealthLevel() << endl;
        cout << "Unrest: " << (population->isUnrestActive() ? "Active" : "None") << endl;
        cout << "Disease Susceptibility: " << population->getDiseaseSusceptibility() << endl;
        cout << "Food Consumption: " << population->getFoodConsumptionPerCapita() << " per capita" << endl;

        cout << "Social Classes:" << endl;
        for (int i = 0; i < population->getNumClasses(); i++) {
            SocialClass* socialClass = population->getSocialClass(i);
            if (socialClass) {
                cout << "- " << socialClass->getName() << ": " << socialClass->getPopulation()
                    << " people (Tax Rate: " << (socialClass->getTaxRate() * 100) << "%)" << endl;
            }
        }
    }

    // Economy information
    Economy* economy = playerKingdom->getEconomy();
    if (economy) {
        cout << "\n--- ECONOMY ---" << endl;
        cout << "Production Level: " << economy->getProductionLevel() << endl;
        cout << "Trade Level: " << economy->getTradeLevel() << endl;
        cout << "Tax Rate: " << economy->getTaxRate() << "%" << endl;
        cout << "Market Stability: " << economy->getMarketStability() << endl;
        cout << "Inflation: " << economy->getInflation() << "%" << endl;
        cout << "Employment Rate: " << economy->getEmploymentRate() << "%" << endl;
        cout << "Corruption Level: " << economy->getCorruptionLevel() << endl;

        cout << "Resources:" << endl;
        for (int i = 0; i < economy->getNumResources(); i++) {
            Resource* resource = economy->getResource(i);
            if (resource) {
                cout << "- " << resource->getName() << ": " << resource->getAmount()
                    << " (Gather Rate: " << resource->getGatherRate()
                    << ", Consumption Rate: " << resource->getConsumptionRate() << ")" << endl;
            }
        }

        Treasury* treasury = economy->getTreasury();
        if (treasury) {
            cout << "\n--- TREASURY ---" << endl;
            cout << "Gold: " << treasury->getGold() << endl;
            cout << "Income: " << treasury->getIncome() << " gold per turn" << endl;
            cout << "Expenses: " << treasury->getExpenses() << " gold per turn" << endl;
            cout << "Balance: " << treasury->calculateBalance() << " gold per turn" << endl;
            cout << "Tax Income: " << treasury->getTaxIncome() << endl;
            cout << "Trade Income: " << treasury->getTradeIncome() << endl;
            cout << "Military Expenses: " << treasury->getMilitaryExpenses() << endl;
            cout << "Building Expenses: " << treasury->getBuildingExpenses() << endl;
            cout << "Treasury Corruption: " << treasury->getCorruption() << endl;
            cout << "Inflation: " << treasury->getInflation() << "%" << endl;
        }
    }

    // Military information
    Army* army = playerKingdom->getArmy();
    if (army) {
        cout << "\n--- ARMY ---" << endl;
        cout << "Name: " << army->getName() << endl;
        cout << "Total Strength: " << army->getTotalStrength() << endl;
        cout << "Overall Morale: " << army->getOverallMorale() << endl;
        cout << "Training Level: " << army->getTrainingLevel() << endl;
        cout << "Food Consumption: " << army->getFoodConsumption() << endl;
        cout << "Maintenance Cost: " << army->calculateMaintenanceCost() << " gold per turn" << endl;

        string strategies[] = { "Balanced", "Aggressive", "Defensive", "Guerrilla" };
        cout << "Current Strategy: " << strategies[static_cast<int>(army->getStrategy())] << endl;

        cout << "Military Units:" << endl;
        for (int i = 0; i < army->getNumUnits(); i++) {
            MilitaryUnit* unit = army->getUnit(i);
            if (unit) {
                cout << "- " << unit->getName() << ": " << unit->getCount() << " soldiers"
                    << " (Power: " << unit->calculatePower()
                    << ", Morale: " << unit->getMorale()
                    << ", Veteran: " << (unit->getIsVeteran() ? "Yes" : "No") << ")" << endl;
            }
        }
    }

    // Weather information
    Weather* weather = playerKingdom->getCurrentWeather();
    if (weather) {
        cout << "\n--- WEATHER ---" << endl;
        cout << "Current Weather: " << weather->getName() << endl;
        cout << "Description: " << weather->getDescription() << endl;
        cout << "Severity: " << weather->getSeverity() << endl;
        cout << "Duration: " << weather->getDuration() << " turns" << endl;
        cout << "Turns Remaining: " << weather->getTurnsRemaining() << endl;
        cout << "Extreme: " << (weather->getIsExtreme() ? "Yes" : "No") << endl;
        cout << "Crop Effect: " << weather->getCropEffect() << endl;
        cout << "Movement Effect: " << weather->getMovementEffect() << endl;
        cout << "Morale Effect: " << weather->getMoraleEffect() << endl;
        cout << "Disease Modifier: " << weather->getDiseaseModifier() << endl;
    }

    // Disease information
    Disease* disease = playerKingdom->getCurrentDisease();
    if (disease && disease->getIsActive()) {
        cout << "\n--- DISEASE ---" << endl;
        cout << "Current Disease: " << disease->getName() << endl;
        cout << "Description: " << disease->getDescription() << endl;
        cout << "Severity: " << disease->getSeverity() << endl;
        cout << "Infectivity: " << disease->getInfectivity() << endl;
        cout << "Mortality Rate: " << disease->getMortalityRate() << "%" << endl;
        cout << "Current Infected: " << disease->getCurrentInfected() << endl;
        cout << "Duration: " << disease->getDuration() << " turns" << endl;
        cout << "Turns Remaining: " << disease->getTurnsRemaining() << endl;
    }

    // Events information
    cout << "\n--- ACTIVE EVENTS ---" << endl;
    if (playerKingdom->getNumEvents() > 0) {
        for (int i = 0; i < playerKingdom->getNumEvents(); i++) {
            Event* event = playerKingdom->getEvent(i);
            if (event) {
                cout << "- " << event->getName() << ": " << event->getDescription() << endl;
                cout << "  Duration: " << event->getDuration() << " turns, Remaining: " << event->getTurnsRemaining() << " turns" << endl;
                cout << "  Effects: Population " << event->getPopulationEffect() << "%, Economy " << event->getEconomyEffect() << "%, Military " << event->getMilitaryEffect() << "%" << endl;
            }
        }
    }
    else {
        cout << "No active events." << endl;
    }

    cout << "\nPress Enter to continue...";
    cin.get();
}

// Economy management interface
// Economy interface for managing economic aspects
void GameEngine::economyInterface() {
    if (!playerKingdom) {
        cout << "No player kingdom found!" << endl;
        return;
    }

    Economy* economy = playerKingdom->getEconomy();
    if (!economy) {
        cout << "Economy system not initialized!" << endl;
        return;
    }

    cout << "\n=== ECONOMY MANAGEMENT ===\n" << endl;
    cout << "1. Adjust Tax Rates" << endl;
    cout << "2. Manage Resources" << endl;
    cout << "3. Banking" << endl;
    cout << "4. Trade" << endl;
    cout << "5. View Economic Report" << endl;
    cout << "6. Return to Main Menu" << endl;

    int choice;
    cout << "Enter your choice: ";
    cin >> choice;

    // Clear input buffer
    cin.clear();
    cin.ignore(1000, '\n');

    switch (choice) {
    case 1:
        adjustTaxRates();
        break;
    case 2:
        manageResources();
        break;
    case 3:
        bankingInterface();
        break;
    case 4:
        manageTradeOptions();
        break;
    case 5:
        viewEconomicReport();
        break;
    case 6:
        return; // Return to main menu
    default:
        cout << "Invalid choice. Please try again." << endl;
        economyInterface(); // Try again
        break;
    }
}

// Adjust tax rates for different social classes
void GameEngine::adjustTaxRates() {
    if (!playerKingdom) return;

    Population* population = playerKingdom->getPopulation();
    if (!population) {
        cout << "Population system not initialized!" << endl;
        return;
    }

    cout << "\n=== ADJUST TAX RATES ===\n" << endl;
    cout << "Current social classes and tax rates:" << endl;

    for (int i = 0; i < population->getNumClasses(); i++) {
        SocialClass* socialClass = population->getSocialClass(i);
        if (socialClass) {
            cout << i + 1 << ". " << socialClass->getName() << ": "
                << (socialClass->getTaxRate() * 100) << "% "
                << "(Population: " << socialClass->getPopulation() << ", "
                << "Happiness: " << socialClass->getHappiness() << ")" << endl;
        }
    }

    int classIndex;
    cout << "\nEnter the number of the class to adjust (0 to cancel): ";
    cin >> classIndex;

    if (classIndex <= 0 || classIndex > population->getNumClasses()) {
        cout << "Returning to Economy Menu..." << endl;
        return;
    }

    SocialClass* selectedClass = population->getSocialClass(classIndex - 1);
    if (!selectedClass) {
        cout << "Invalid class selection." << endl;
        return;
    }

    double currentRate = selectedClass->getTaxRate();
    double newRate;

    cout << "Current tax rate for " << selectedClass->getName() << ": " << (currentRate * 100) << "%" << endl;
    cout << "Enter new tax rate (0-100): ";
    cin >> newRate;

    // Convert percentage to decimal
    newRate /= 100.0;

    if (newRate < 0.0) newRate = 0.0;
    if (newRate > 1.0) newRate = 1.0;

    selectedClass->setTaxRate(newRate);

    cout << "Tax rate for " << selectedClass->getName() << " adjusted to " << (newRate * 100) << "%" << endl;

    // Display warning if tax rate is high
    if (newRate > 0.5) {
        cout << "WARNING: High tax rates may cause unhappiness and unrest!" << endl;
    }

    cout << "\nPress Enter to continue...";
    cin.ignore(); // Clear previous input
    cin.get();
}

// Resource management interface
void GameEngine::manageResources() {
    if (!playerKingdom) return;

    Economy* economy = playerKingdom->getEconomy();
    if (!economy) {
        cout << "Economy system not initialized!" << endl;
        return;
    }

    bool exitMenu = false;

    while (!exitMenu) {
        cout << "\n=== RESOURCE MANAGEMENT ===\n" << endl;
        cout << "Current Resources:" << endl;

        for (int i = 0; i < economy->getNumResources(); i++) {
            Resource* resource = economy->getResource(i);
            if (resource) {
                cout << i + 1 << ". " << resource->getName()
                    << " (" << resource->getAmount() << ")"
                    << " - Gather rate: " << resource->getGatherRate()
                    << ", Consumption: " << resource->getConsumptionRate() << endl;
            }
        }

        cout << "\nOptions:" << endl;
        cout << "1. Adjust Gather Rates" << endl;
        cout << "2. Adjust Consumption Rates" << endl;
        cout << "3. Trade Resources" << endl;
        cout << "4. Stockpile Resources" << endl;
        cout << "5. Return to Economy Menu" << endl;

        int choice;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice) {
        case 1: {
            cout << "\n=== ADJUST GATHER RATES ===\n" << endl;
            cout << "Select a resource to adjust (0 to cancel): ";
            int resIndex;
            cin >> resIndex;
            cin.ignore(1000, '\n');

            if (resIndex <= 0 || resIndex > economy->getNumResources()) {
                cout << "Returning to Resource Management..." << endl;
            }
            else {
                Resource* resource = economy->getResource(resIndex - 1);
                if (resource) {
                    cout << "Current gather rate for " << resource->getName() << ": " << resource->getGatherRate() << endl;
                    cout << "Enter new gather rate: ";
                    int newRate;
                    cin >> newRate;
                    cin.ignore(1000, '\n');

                    if (newRate < 0) newRate = 0;
                    resource->setGatherRate(newRate);
                    cout << "Gather rate for " << resource->getName() << " set to " << newRate << endl;
                }
            }
            break;
        }
        case 2: {
            cout << "\n=== ADJUST CONSUMPTION RATES ===\n" << endl;
            cout << "Select a resource to adjust (0 to cancel): ";
            int resIndex;
            cin >> resIndex;
            cin.ignore(1000, '\n');

            if (resIndex <= 0 || resIndex > economy->getNumResources()) {
                cout << "Returning to Resource Management..." << endl;
            }
            else {
                Resource* resource = economy->getResource(resIndex - 1);
                if (resource) {
                    cout << "Current consumption rate for " << resource->getName() << ": " << resource->getConsumptionRate() << endl;
                    cout << "Enter new consumption rate: ";
                    int newRate;
                    cin >> newRate;
                    cin.ignore(1000, '\n');

                    if (newRate < 0) newRate = 0;
                    resource->setConsumptionRate(newRate);
                    cout << "Consumption rate for " << resource->getName() << " set to " << newRate << endl;
                }
            }
            break;
        }
        case 3: {
            cout << "\n=== TRADE RESOURCES ===\n" << endl;

            // Select resource to sell
            cout << "Select resource to sell (0 to cancel): ";
            int sellIndex;
            cin >> sellIndex;
            cin.ignore(1000, '\n');

            if (sellIndex <= 0 || sellIndex > economy->getNumResources()) {
                cout << "Returning to Resource Management..." << endl;
                break;
            }

            Resource* sellResource = economy->getResource(sellIndex - 1);
            if (!sellResource) {
                cout << "Invalid resource selection." << endl;
                break;
            }

            cout << "Amount to sell (available: " << sellResource->getAmount() << "): ";
            int sellAmount;
            cin >> sellAmount;
            cin.ignore(1000, '\n');

            if (sellAmount <= 0 || sellAmount > sellResource->getAmount()) {
                cout << "Invalid amount." << endl;
                break;
            }

            // Select resource to buy
            cout << "Select resource to buy (0 to cancel): ";
            int buyIndex;
            cin >> buyIndex;
            cin.ignore(1000, '\n');

            if (buyIndex <= 0 || buyIndex > economy->getNumResources() || buyIndex == sellIndex) {
                cout << "Returning to Resource Management..." << endl;
                break;
            }

            Resource* buyResource = economy->getResource(buyIndex - 1);
            if (!buyResource) {
                cout << "Invalid resource selection." << endl;
                break;
            }

            // Calculate trade rate based on resource values
            int buyAmount = (sellAmount * sellResource->getValue()) / buyResource->getValue();
            buyAmount = max(1, buyAmount); // Ensure at least 1 unit

            cout << "You will receive " << buyAmount << " " << buyResource->getName()
                << " for " << sellAmount << " " << sellResource->getName() << endl;
            cout << "Confirm trade? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                bool success = economy->trade(sellResource->getType(), sellAmount,
                    buyResource->getType(), buyAmount);
                if (success) {
                    cout << "Trade successful!" << endl;
                }
                else {
                    cout << "Trade failed. Please check your resources." << endl;
                }
            }
            else {
                cout << "Trade canceled." << endl;
            }
            break;
        }
        case 4: {
            cout << "\n=== STOCKPILE RESOURCES ===\n" << endl;
            cout << "This option allows you to prioritize resource storage for future use.\n";

            cout << "Select a resource to stockpile (0 to cancel): ";
            int resIndex;
            cin >> resIndex;
            cin.ignore(1000, '\n');

            if (resIndex <= 0 || resIndex > economy->getNumResources()) {
                cout << "Returning to Resource Management..." << endl;
            }
            else {
                Resource* resource = economy->getResource(resIndex - 1);
                if (resource) {
                    cout << "Current stockpile priority for " << resource->getName() << ": "
                        << (resource->getIsStockpiled() ? "High" : "Normal") << endl;
                    cout << "Set priority? (1 for High, 0 for Normal): ";
                    int priority;
                    cin >> priority;
                    cin.ignore(1000, '\n');

                    resource->setIsStockpiled(priority == 1);
                    cout << "Stockpile priority for " << resource->getName()
                        << " set to " << (priority == 1 ? "High" : "Normal") << endl;

                    // When stockpiled, consumption is reduced
                    if (priority == 1) {
                        int reduced = resource->getConsumptionRate() * 0.7;
                        resource->setConsumptionRate(reduced);
                        cout << "Consumption rate reduced to " << reduced
                            << " due to stockpiling." << endl;
                    }
                }
            }
            break;
        }
        case 5:
        default:
            exitMenu = true;
            break;
        }

        if (!exitMenu) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
}

// Banking interface
void GameEngine::bankingInterface() {
    if (!playerKingdom) return;

    Economy* economy = playerKingdom->getEconomy();
    if (!economy) {
        cout << "Economy system not initialized!" << endl;
        return;
    }

    Bank* bank = economy->getBank();
    if (!bank) {
        cout << "Banking system not initialized!" << endl;
        return;
    }

    Treasury* treasury = economy->getTreasury();
    if (!treasury) {
        cout << "Treasury system not initialized!" << endl;
        return;
    }

    bool exitMenu = false;

    while (!exitMenu) {
        cout << "\n=== BANKING INTERFACE ===\n" << endl;
        cout << "Royal Treasury: " << treasury->getGold() << " gold" << endl;
        cout << "Bank Reserve: " << bank->getReserve() << " gold" << endl;
        cout << "Loan Interest Rate: " << (bank->getInterestRate() * 100) << "%" << endl;
        cout << "Investment Return Rate: " << (bank->getInvestmentReturnRate() * 100) << "%" << endl;
        cout << "Current Active Loans: " << bank->getNumLoans() << endl;
        cout << "Current Investments: " << bank->getNumInvestments() << endl;

        cout << "\nOptions:" << endl;
        cout << "1. Take a Loan" << endl;
        cout << "2. Repay Loans" << endl;
        cout << "3. Make Investment" << endl;
        cout << "4. Adjust Interest Rates" << endl;
        cout << "5. View Loan and Investment Details" << endl;
        cout << "6. Return to Economy Menu" << endl;

        int choice;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice) {
        case 1: {
            cout << "\n=== TAKE A LOAN ===\n" << endl;
            cout << "Royal Treasury: " << treasury->getGold() << " gold" << endl;
            cout << "Bank Reserve: " << bank->getReserve() << " gold" << endl;
            cout << "Loan Interest Rate: " << (bank->getInterestRate() * 100) << "%" << endl;

            int amount;
            cout << "Enter loan amount (max " << bank->getReserve() << " gold): ";
            cin >> amount;
            cin.ignore(1000, '\n');

            if (amount <= 0) {
                cout << "Invalid amount." << endl;
                break;
            }

            if (amount > bank->getReserve()) {
                cout << "The bank does not have enough gold for this loan." << endl;
                break;
            }

            int term;
            cout << "Enter loan term in turns (5-50): ";
            cin >> term;
            cin.ignore(1000, '\n');

            if (term < 5 || term > 50) {
                cout << "Invalid term. Choose between 5 and 50 turns." << endl;
                break;
            }

            // Create the loan
            double interestRate = bank->getInterestRate();
            int totalRepayment = static_cast<int>(amount * (1 + interestRate * term / 10.0));

            cout << "Loan summary:" << endl;
            cout << "Amount: " << amount << " gold" << endl;
            cout << "Term: " << term << " turns" << endl;
            cout << "Interest rate: " << (interestRate * 100) << "%" << endl;
            cout << "Total repayment: " << totalRepayment << " gold" << endl;
            cout << "Payment per turn: " << (totalRepayment / term) << " gold" << endl;

            cout << "Confirm loan? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                bool success = bank->provideLoan(amount, term, treasury);
                if (success) {
                    cout << "Loan approved! " << amount << " gold added to treasury." << endl;
                }
                else {
                    cout << "Loan failed. Please check bank reserves." << endl;
                }
            }
            else {
                cout << "Loan canceled." << endl;
            }
            break;
        }
        case 2: {
            cout << "\n=== REPAY LOANS ===\n" << endl;
            cout << "Royal Treasury: " << treasury->getGold() << " gold" << endl;

            // Display active loans
            if (bank->getNumLoans() == 0) {
                cout << "No active loans to repay." << endl;
                break;
            }

            cout << "Active loans:" << endl;
            int totalLoanDebt = bank->displayLoans();

            cout << "\nTotal loan debt: " << totalLoanDebt << " gold" << endl;

            cout << "How much would you like to repay? (0 to cancel): ";
            int repayAmount;
            cin >> repayAmount;
            cin.ignore(1000, '\n');

            if (repayAmount <= 0) {
                cout << "Payment canceled." << endl;
                break;
            }

            if (repayAmount > treasury->getGold()) {
                cout << "You don't have enough gold in the treasury." << endl;
                break;
            }

            int repaid = bank->repayLoans(repayAmount, treasury);
            cout << repaid << " gold repaid toward loans." << endl;
            break;
        }
        case 3: {
            cout << "\n=== MAKE INVESTMENT ===\n" << endl;
            cout << "Royal Treasury: " << treasury->getGold() << " gold" << endl;
            cout << "Investment Return Rate: " << (bank->getInvestmentReturnRate() * 100) << "%" << endl;

            int amount;
            cout << "Enter investment amount (max " << treasury->getGold() << " gold): ";
            cin >> amount;
            cin.ignore(1000, '\n');

            if (amount <= 0) {
                cout << "Invalid amount." << endl;
                break;
            }

            if (amount > treasury->getGold()) {
                cout << "You don't have enough gold in the treasury." << endl;
                break;
            }

            int term;
            cout << "Enter investment term in turns (10-100): ";
            cin >> term;
            cin.ignore(1000, '\n');

            if (term < 10 || term > 100) {
                cout << "Invalid term. Choose between 10 and 100 turns." << endl;
                break;
            }

            // Calculate returns
            double returnRate = bank->getInvestmentReturnRate();
            int totalReturn = static_cast<int>(amount * (1 + returnRate * term / 10.0));

            cout << "Investment summary:" << endl;
            cout << "Amount: " << amount << " gold" << endl;
            cout << "Term: " << term << " turns" << endl;
            cout << "Return rate: " << (returnRate * 100) << "%" << endl;
            cout << "Total expected return: " << totalReturn << " gold (+"
                << (totalReturn - amount) << " profit)" << endl;

            cout << "Confirm investment? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                bool success = bank->makeInvestment(amount, term, treasury);
                if (success) {
                    cout << "Investment made successfully! " << amount << " gold invested." << endl;
                }
                else {
                    cout << "Investment failed. Please check your treasury." << endl;
                }
            }
            else {
                cout << "Investment canceled." << endl;
            }
            break;
        }
        case 4: {
            cout << "\n=== ADJUST INTEREST RATES ===\n" << endl;
            cout << "Current loan interest rate: " << (bank->getInterestRate() * 100) << "%" << endl;
            cout << "Current investment return rate: " << (bank->getInvestmentReturnRate() * 100) << "%" << endl;

            cout << "Enter new loan interest rate (0-25%): ";
            double newRate;
            cin >> newRate;
            cin.ignore(1000, '\n');

            // Convert percentage to decimal
            newRate /= 100.0;

            if (newRate < 0.0) newRate = 0.0;
            if (newRate > 0.25) newRate = 0.25;

            bank->setInterestRate(newRate);

            // Investment return rate is typically lower than loan rate
            double newInvestRate = newRate * 0.7;
            bank->setInvestmentReturnRate(newInvestRate);

            cout << "Rates adjusted:" << endl;
            cout << "Loan interest rate: " << (bank->getInterestRate() * 100) << "%" << endl;
            cout << "Investment return rate: " << (bank->getInvestmentReturnRate() * 100) << "%" << endl;

            if (newRate > 0.15) {
                cout << "WARNING: High interest rates may discourage borrowing and slow economic growth!" << endl;
            }
            else if (newRate < 0.05) {
                cout << "WARNING: Low interest rates may lead to excessive borrowing and inflation!" << endl;
            }
            break;
        }
        case 5: {
            cout << "\n=== LOAN AND INVESTMENT DETAILS ===\n" << endl;

            cout << "Active Loans:" << endl;
            if (bank->getNumLoans() == 0) {
                cout << "No active loans." << endl;
            }
            else {
                bank->displayLoans();
            }

            cout << "\nActive Investments:" << endl;
            if (bank->getNumInvestments() == 0) {
                cout << "No active investments." << endl;
            }
            else {
                bank->displayInvestments();
            }
            break;
        }
        case 6:
        default:
            exitMenu = true;
            break;
        }

        if (!exitMenu) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
}

// Trade options management
void GameEngine::manageTradeOptions() {
    if (!playerKingdom) return;

    Economy* economy = playerKingdom->getEconomy();
    if (!economy) {
        cout << "Economy system not initialized!" << endl;
        return;
    }

    bool exitMenu = false;

    while (!exitMenu) {
        cout << "\n=== TRADE MANAGEMENT ===\n" << endl;
        cout << "Current Trade Level: " << economy->getTradeLevel() << endl;
        cout << "Market Stability: " << economy->getMarketStability() << endl;
        cout << "Trade Routes: " << economy->getNumTradeRoutes() << " / " << economy->getMaxTradeRoutes() << endl;

        cout << "\nOptions:" << endl;
        cout << "1. Establish New Trade Route" << endl;
        cout << "2. View Current Trade Routes" << endl;
        cout << "3. Cancel Trade Route" << endl;
        cout << "4. Adjust Tariffs" << endl;
        cout << "5. View Market Prices" << endl;
        cout << "6. Return to Economy Menu" << endl;

        int choice;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice) {
        case 1: {
            cout << "\n=== ESTABLISH NEW TRADE ROUTE ===\n" << endl;

            if (economy->getNumTradeRoutes() >= economy->getMaxTradeRoutes()) {
                cout << "Maximum number of trade routes reached!" << endl;
                cout << "You must cancel an existing route before establishing a new one." << endl;
                break;
            }

            cout << "Available kingdoms to trade with:" << endl;

            // In this version we'll use dummy kingdoms for simplicity
            string kingdomNames[] = { "Northern Realms", "Eastern Empire", "Southern Sultanate", "Western Republic", "Island Nation" };

            for (int i = 0; i < 5; i++) {
                cout << i + 1 << ". " << kingdomNames[i] << endl;
            }

            int kingdomChoice;
            cout << "\nSelect a kingdom to establish trade with (0 to cancel): ";
            cin >> kingdomChoice;
            cin.ignore(1000, '\n');

            if (kingdomChoice <= 0 || kingdomChoice > 5) {
                cout << "Canceled." << endl;
                break;
            }

            string selectedKingdom = kingdomNames[kingdomChoice - 1];

            // Select resources to trade
            cout << "\nSelect resource to export:" << endl;
            for (int i = 0; i < economy->getNumResources(); i++) {
                Resource* resource = economy->getResource(i);
                if (resource) {
                    cout << i + 1 << ". " << resource->getName()
                        << " (Available: " << resource->getAmount() << ")" << endl;
                }
            }

            int exportChoice;
            cout << "\nExport resource (0 to cancel): ";
            cin >> exportChoice;
            cin.ignore(1000, '\n');

            if (exportChoice <= 0 || exportChoice > economy->getNumResources()) {
                cout << "Canceled." << endl;
                break;
            }

            Resource* exportResource = economy->getResource(exportChoice - 1);
            if (!exportResource) {
                cout << "Invalid resource selection." << endl;
                break;
            }

            cout << "\nSelect resource to import:" << endl;
            for (int i = 0; i < economy->getNumResources(); i++) {
                if (i != exportChoice - 1) { // Skip the export resource
                    Resource* resource = economy->getResource(i);
                    if (resource) {
                        cout << i + 1 << ". " << resource->getName() << endl;
                    }
                }
            }

            int importChoice;
            cout << "\nImport resource (0 to cancel): ";
            cin >> importChoice;
            cin.ignore(1000, '\n');

            if (importChoice <= 0 || importChoice > economy->getNumResources() || importChoice == exportChoice) {
                cout << "Canceled." << endl;
                break;
            }

            Resource* importResource = economy->getResource(importChoice - 1);
            if (!importResource) {
                cout << "Invalid resource selection." << endl;
                break;
            }

            // Set trade amounts
            int exportAmount;
            cout << "Amount to export per turn (available: " << exportResource->getAmount() << "): ";
            cin >> exportAmount;
            cin.ignore(1000, '\n');

            if (exportAmount <= 0 || exportAmount > exportResource->getAmount()) {
                cout << "Invalid amount." << endl;
                break;
            }

            // Calculate trade ratio based on value
            double ratio = static_cast<double>(exportResource->getValue()) / static_cast<double>(importResource->getValue());
            int importAmount = static_cast<int>(exportAmount * ratio);

            cout << "\nTrade Route Summary:" << endl;
            cout << "Partner: " << selectedKingdom << endl;
            cout << "Export: " << exportAmount << " " << exportResource->getName() << " per turn" << endl;
            cout << "Import: " << importAmount << " " << importResource->getName() << " per turn" << endl;
            cout << "Cost to establish: " << (50 + exportAmount * 2) << " gold" << endl;

            cout << "\nConfirm trade route? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                int cost = 50 + exportAmount * 2;
                Treasury* treasury = economy->getTreasury();
                if (!treasury) {
                    cout << "Treasury not initialized!" << endl;
                    break;
                }

                if (treasury->getGold() < cost) {
                    cout << "Not enough gold in treasury to establish trade route!" << endl;
                    break;
                }

                treasury->spend(cost);

                // Establish the trade route
                economy->addTradeRoute(selectedKingdom,
                    exportResource->getType(), exportAmount,
                    importResource->getType(), importAmount);

                cout << "Trade route established with " << selectedKingdom << "!" << endl;
            }
            else {
                cout << "Trade route canceled." << endl;
            }
            break;
        }
        case 2: {
            cout << "\n=== CURRENT TRADE ROUTES ===\n" << endl;

            if (economy->getNumTradeRoutes() == 0) {
                cout << "No active trade routes." << endl;
                break;
            }

            economy->displayTradeRoutes();
            break;
        }
        case 3: {
            cout << "\n=== CANCEL TRADE ROUTE ===\n" << endl;

            if (economy->getNumTradeRoutes() == 0) {
                cout << "No active trade routes to cancel." << endl;
                break;
            }

            // Display routes with indices
            economy->displayTradeRoutes();

            int routeIndex;
            cout << "\nSelect trade route to cancel (0 to return): ";
            cin >> routeIndex;
            cin.ignore(1000, '\n');

            if (routeIndex <= 0 || routeIndex > economy->getNumTradeRoutes()) {
                cout << "Canceled." << endl;
                break;
            }

            cout << "Are you sure you want to cancel this trade route? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                economy->removeTradeRoute(routeIndex - 1);
                cout << "Trade route canceled successfully." << endl;
            }
            else {
                cout << "Operation canceled." << endl;
            }
            break;
        }
        case 4: {
            cout << "\n=== ADJUST TARIFFS ===\n" << endl;
            cout << "Current tariff rate: " << (economy->getTariffRate() * 100) << "%" << endl;

            cout << "Enter new tariff rate (0-50%): ";
            double newRate;
            cin >> newRate;
            cin.ignore(1000, '\n');

            // Convert percentage to decimal
            newRate /= 100.0;

            if (newRate < 0.0) newRate = 0.0;
            if (newRate > 0.5) newRate = 0.5;

            economy->setTariffRate(newRate);

            cout << "Tariff rate adjusted to " << (newRate * 100) << "%" << endl;

            if (newRate > 0.3) {
                cout << "WARNING: High tariffs may reduce trade volume and diplomatic relations!" << endl;
            }
            else if (newRate < 0.05) {
                cout << "WARNING: Low tariffs reduce treasury income from trade!" << endl;
            }
            break;
        }
        case 5: {
            cout << "\n=== MARKET PRICES ===\n" << endl;

            cout << "Current resource values:" << endl;
            for (int i = 0; i < economy->getNumResources(); i++) {
                Resource* resource = economy->getResource(i);
                if (resource) {
                    cout << "- " << resource->getName() << ": " << resource->getValue() << " gold per unit" << endl;
                }
            }

            // Allow adjustment of prices for testing
            cout << "\nWould you like to adjust market prices? (1 for yes, 0 for no): ";
            int adjust;
            cin >> adjust;
            cin.ignore(1000, '\n');

            if (adjust == 1) {
                cout << "Select resource to adjust (0 to cancel): ";
                int resIndex;
                cin >> resIndex;
                cin.ignore(1000, '\n');

                if (resIndex <= 0 || resIndex > economy->getNumResources()) {
                    cout << "Canceled." << endl;
                    break;
                }

                Resource* resource = economy->getResource(resIndex - 1);
                if (resource) {
                    cout << "Current value for " << resource->getName() << ": " << resource->getValue() << endl;
                    cout << "Enter new value (1-100): ";
                    int newValue;
                    cin >> newValue;
                    cin.ignore(1000, '\n');

                    if (newValue < 1) newValue = 1;
                    if (newValue > 100) newValue = 100;

                    resource->setValue(newValue);
                    cout << "Market value for " << resource->getName() << " set to " << newValue << endl;
                }
            }
            break;
        }
        case 6:
        default:
            exitMenu = true;
            break;
        }

        if (!exitMenu) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
}

// Economic report view
void GameEngine::viewEconomicReport() {
    if (!playerKingdom) return;

    Economy* economy = playerKingdom->getEconomy();
    if (!economy) {
        cout << "Economy system not initialized!" << endl;
        return;
    }

    Treasury* treasury = economy->getTreasury();
    if (!treasury) {
        cout << "Treasury system not initialized!" << endl;
        return;
    }

    cout << "\n=== ECONOMIC REPORT ===\n" << endl;
    cout << "Treasury: " << treasury->getGold() << " gold" << endl;
    cout << "Income: " << treasury->getIncome() << " gold per turn" << endl;
    cout << "Expenses: " << treasury->getExpenses() << " gold per turn" << endl;
    cout << "Balance: " << treasury->calculateBalance() << " gold per turn" << endl;
    cout << "Production Level: " << economy->getProductionLevel() << endl;
    cout << "Trade Level: " << economy->getTradeLevel() << endl;
    cout << "Market Stability: " << economy->getMarketStability() << endl;
    cout << "Inflation: " << economy->getInflation() << "%" << endl;
    cout << "Employment Rate: " << economy->getEmploymentRate() << "%" << endl;
    cout << "Corruption Level: " << economy->getCorruptionLevel() << endl;

    cout << "\nResources:" << endl;
    for (int i = 0; i < economy->getNumResources(); i++) {
        Resource* resource = economy->getResource(i);
        if (resource) {
            cout << "- " << resource->getName() << ": " << resource->getAmount() << endl;
            cout << "  Gather Rate: " << resource->getGatherRate() << " per turn" << endl;
            cout << "  Consumption Rate: " << resource->getConsumptionRate() << " per turn" << endl;
            cout << "  Net Change: " << (resource->getGatherRate() - resource->getConsumptionRate()) << " per turn" << endl;
        }
    }

    cout << "\nPress Enter to continue...";
    cin.get();
}

// Military management interface
void GameEngine::militaryInterface() {
    if (!playerKingdom) return;

    Army* army = playerKingdom->getArmy();
    if (!army) {
        cout << "Military system not initialized!" << endl;
        return;
    }

    Economy* economy = playerKingdom->getEconomy();
    if (!economy) {
        cout << "Economy system not initialized!" << endl;
        return;
    }

    Treasury* treasury = economy->getTreasury();
    if (!treasury) {
        cout << "Treasury system not initialized!" << endl;
        return;
    }

    bool exitMenu = false;

    while (!exitMenu) {
        cout << "\n=== MILITARY INTERFACE ===\n" << endl;
        cout << "Army Strength: " << army->getTotalStrength() << endl;
        cout << "Morale: " << army->getMorale() << "%" << endl;
        cout << "Discipline: " << army->getDiscipline() << "%" << endl;
        cout << "Total Units: " << army->getNumUnits() << endl;
        cout << "Maintenance Cost: " << army->getMaintenanceCost() << " gold per turn" << endl;

        cout << "\nOptions:" << endl;
        cout << "1. Recruit Units" << endl;
        cout << "2. Train Army" << endl;
        cout << "3. Disband Units" << endl;
        cout << "4. View Units" << endl;
        cout << "5. Plan Battle" << endl;
        cout << "6. Return to Main Menu" << endl;

        int choice;
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice) {
        case 1: {
            cout << "\n=== RECRUIT UNITS ===\n" << endl;
            cout << "Treasury: " << treasury->getGold() << " gold" << endl;

            cout << "Available unit types:" << endl;
            cout << "1. Infantry (50 gold)" << endl;
            cout << "2. Archers (75 gold)" << endl;
            cout << "3. Cavalry (100 gold)" << endl;
            cout << "4. Special Forces (150 gold)" << endl;

            int unitType;
            cout << "\nSelect unit type to recruit (0 to cancel): ";
            cin >> unitType;
            cin.ignore(1000, '\n');

            if (unitType <= 0 || unitType > 4) {
                cout << "Canceled." << endl;
                break;
            }

            int cost = 0;
            string unitName;
            int attack = 0, defense = 0, speed = 0;

            switch (unitType) {
            case 1: // Infantry
                unitName = "Infantry";
                cost = 50;
                attack = 10;
                defense = 15;
                speed = 5;
                break;
            case 2: // Archers
                unitName = "Archers";
                cost = 75;
                attack = 15;
                defense = 5;
                speed = 8;
                break;
            case 3: // Cavalry
                unitName = "Cavalry";
                cost = 100;
                attack = 20;
                defense = 10;
                speed = 15;
                break;
            case 4: // Special Forces
                unitName = "Royal Guards";
                cost = 150;
                attack = 25;
                defense = 20;
                speed = 10;
                break;
            }

            int quantity;
            cout << "How many " << unitName << " units do you want to recruit? ";
            cin >> quantity;
            cin.ignore(1000, '\n');

            if (quantity <= 0) {
                cout << "Invalid quantity." << endl;
                break;
            }

            int totalCost = cost * quantity;

            cout << "\nRecruitment summary:" << endl;
            cout << quantity << " " << unitName << " units" << endl;
            cout << "Unit stats: Attack " << attack << ", Defense " << defense << ", Speed " << speed << endl;
            cout << "Total cost: " << totalCost << " gold" << endl;

            cout << "Confirm recruitment? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                if (treasury->getGold() < totalCost) {
                    cout << "Not enough gold in treasury!" << endl;
                    break;
                }

                // Deduct gold
                treasury->spend(totalCost);

                // Add units
                for (int i = 0; i < quantity; i++) {
                    string unitId = unitName + " #" + to_string(army->getNumUnits() + 1);
                    string unitFullName = "Standard " + unitName;
                    int soldierCount = 10; // Default count of soldiers in this unit
                    MilitaryUnit* unit = new MilitaryUnit(unitId, soldierCount, attack, defense, speed);
                    army->addUnit(unit);
                }

                cout << quantity << " " << unitName << " units recruited successfully!" << endl;

                // Adjust army morale and discipline
                if (army->getMorale() < 95) {
                    army->setMorale(army->getMorale() + 5);
                }
            }
            else {
                cout << "Recruitment canceled." << endl;
            }
            break;
        }
        case 2: {
            cout << "\n=== TRAIN ARMY ===\n" << endl;
            cout << "Current stats:" << endl;
            cout << "Morale: " << army->getMorale() << "%" << endl;
            cout << "Discipline: " << army->getDiscipline() << "%" << endl;
            cout << "Training level: " << army->getTrainingLevel() << endl;

            cout << "\nTraining options:" << endl;
            cout << "1. Basic Training (50 gold) - Improves discipline" << endl;
            cout << "2. Advanced Maneuvers (100 gold) - Improves combat effectiveness" << endl;
            cout << "3. Special Tactics (150 gold) - Improves strategy and morale" << endl;

            int trainingType;
            cout << "\nSelect training type (0 to cancel): ";
            cin >> trainingType;
            cin.ignore(1000, '\n');

            if (trainingType <= 0 || trainingType > 3) {
                cout << "Canceled." << endl;
                break;
            }

            int cost = 0;
            int disciplineBonus = 0;
            int moraleBonus = 0;
            int strengthBonus = 0;

            switch (trainingType) {
            case 1: // Basic Training
                cost = 50;
                disciplineBonus = 10;
                moraleBonus = 5;
                strengthBonus = 5;
                break;
            case 2: // Advanced Maneuvers
                cost = 100;
                disciplineBonus = 5;
                moraleBonus = 10;
                strengthBonus = 10;
                break;
            case 3: // Special Tactics
                cost = 150;
                disciplineBonus = 10;
                moraleBonus = 15;
                strengthBonus = 15;
                break;
            }

            cout << "\nTraining summary:" << endl;
            cout << "Cost: " << cost << " gold" << endl;
            cout << "Discipline bonus: +" << disciplineBonus << "%" << endl;
            cout << "Morale bonus: +" << moraleBonus << "%" << endl;
            cout << "Overall strength bonus: +" << strengthBonus << "%" << endl;

            cout << "Confirm training? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                if (treasury->getGold() < cost) {
                    cout << "Not enough gold in treasury!" << endl;
                    break;
                }

                // Deduct gold
                treasury->spend(cost);

                // Apply bonuses
                army->setDiscipline(min(100, army->getDiscipline() + disciplineBonus));
                army->setMorale(min(100, army->getMorale() + moraleBonus));
                army->setTrainingLevel(army->getTrainingLevel() + 1);

                // Improve individual units
                for (int i = 0; i < army->getNumUnits(); i++) {
                    MilitaryUnit* unit = army->getUnit(i);
                    if (unit) {
                        int currentAttack = unit->getAttack();
                        int currentDefense = unit->getDefense();

                        // Increase stats based on strength bonus
                        int attackIncrease = max(1, currentAttack * strengthBonus / 100);
                        int defenseIncrease = max(1, currentDefense * strengthBonus / 100);

                        unit->setAttack(currentAttack + attackIncrease);
                        unit->setDefense(currentDefense + defenseIncrease);
                        unit->train();
                    }
                }

                cout << "Army training completed successfully!" << endl;
                cout << "All units have improved their combat abilities." << endl;
            }
            else {
                cout << "Training canceled." << endl;
            }
            break;
        }
        case 3: {
            cout << "\n=== DISBAND UNITS ===\n" << endl;

            if (army->getNumUnits() == 0) {
                cout << "No units to disband." << endl;
                break;
            }

            cout << "Current units:" << endl;
            for (int i = 0; i < army->getNumUnits(); i++) {
                MilitaryUnit* unit = army->getUnit(i);
                if (unit) {
                    cout << (i + 1) << ". " << unit->getName()
                        << " - Attack: " << unit->getAttack()
                        << ", Defense: " << unit->getDefense()
                        << ", Speed: " << unit->getSpeed() << endl;
                }
            }

            cout << "\nEnter unit number to disband (0 to cancel): ";
            int unitIndex;
            cin >> unitIndex;
            cin.ignore(1000, '\n');

            if (unitIndex <= 0 || unitIndex > army->getNumUnits()) {
                cout << "Canceled." << endl;
                break;
            }

            MilitaryUnit* selectedUnit = army->getUnit(unitIndex - 1);
            if (!selectedUnit) {
                cout << "Invalid unit selection." << endl;
                break;
            }

            cout << "Are you sure you want to disband " << selectedUnit->getName()
                << "? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                string unitName = selectedUnit->getName();
                army->removeUnit(unitIndex - 1);
                cout << unitName << " has been disbanded." << endl;

                // Adjust army morale slightly downward
                if (army->getMorale() > 5) {
                    army->setMorale(army->getMorale() - 5);
                }
            }
            else {
                cout << "Disbandment canceled." << endl;
            }
            break;
        }
        case 4: {
            cout << "\n=== VIEW UNITS ===\n" << endl;

            if (army->getNumUnits() == 0) {
                cout << "No units in your army." << endl;
                break;
            }

            cout << "Current units:" << endl;
            for (int i = 0; i < army->getNumUnits(); i++) {
                MilitaryUnit* unit = army->getUnit(i);
                if (unit) {
                    cout << (i + 1) << ". " << unit->getName() << endl;
                    cout << "   Type: " << unit->getDescription() << endl;
                    cout << "   Attack: " << unit->getAttack() << endl;
                    cout << "   Defense: " << unit->getDefense() << endl;
                    cout << "   Speed: " << unit->getSpeed() << endl;
                    cout << "   Power: " << unit->calculatePower() << endl;
                    cout << "   Training Level: " << unit->getTrainingLevel() << endl;
                    cout << endl;
                }
            }
            break;
        }
        case 5: {
            cout << "\n=== PLAN BATTLE ===\n" << endl;
            cout << "Select battle type:" << endl;
            cout << "1. Raid Neighboring Village" << endl;
            cout << "2. Border Skirmish" << endl;
            cout << "3. Major Battle" << endl;

            int battleType;
            cout << "\nSelect battle type (0 to cancel): ";
            cin >> battleType;
            cin.ignore(1000, '\n');

            if (battleType <= 0 || battleType > 3) {
                cout << "Canceled." << endl;
                break;
            }

            string battleName;
            int enemyStrength;
            int goldReward;
            int resourceReward;
            int casualtyRisk;

            switch (battleType) {
            case 1: // Raid
                battleName = "Raid on Neighboring Village";
                enemyStrength = army->getTotalStrength() * 0.6;
                goldReward = 200 + rand() % 300;
                resourceReward = 50 + rand() % 100;
                casualtyRisk = 20; // 20% chance of unit loss
                break;
            case 2: // Skirmish
                battleName = "Border Skirmish";
                enemyStrength = army->getTotalStrength() * 0.9;
                goldReward = 500 + rand() % 500;
                resourceReward = 100 + rand() % 200;
                casualtyRisk = 40; // 40% chance of unit loss
                break;
            case 3: // Major Battle
                battleName = "Major Battle";
                enemyStrength = army->getTotalStrength() * 1.2;
                goldReward = 1000 + rand() % 1000;
                resourceReward = 300 + rand() % 300;
                casualtyRisk = 60; // 60% chance of unit loss
                break;
            }

            cout << "\nBattle plan:" << endl;
            cout << "Type: " << battleName << endl;
            cout << "Your army strength: " << army->getTotalStrength() << endl;
            cout << "Estimated enemy strength: " << enemyStrength << endl;
            cout << "Potential reward: " << goldReward << " gold, " << resourceReward << " resources" << endl;
            cout << "Casualty risk: " << casualtyRisk << "%" << endl;

            cout << "\nSelect battle strategy:" << endl;
            cout << "1. Aggressive (higher casualties, higher rewards)" << endl;
            cout << "2. Balanced (moderate casualties, moderate rewards)" << endl;
            cout << "3. Defensive (lower casualties, lower rewards)" << endl;

            int strategy;
            cout << "\nSelect strategy (0 to cancel): ";
            cin >> strategy;
            cin.ignore(1000, '\n');

            if (strategy <= 0 || strategy > 3) {
                cout << "Battle canceled." << endl;
                break;
            }

            // Apply strategy modifiers
            float strengthMod = 1.0f;
            float rewardMod = 1.0f;
            float casualtyMod = 1.0f;

            switch (strategy) {
            case 1: // Aggressive
                strengthMod = 1.2f;
                rewardMod = 1.3f;
                casualtyMod = 1.5f;
                break;
            case 2: // Balanced
                strengthMod = 1.0f;
                rewardMod = 1.0f;
                casualtyMod = 1.0f;
                break;
            case 3: // Defensive
                strengthMod = 0.8f;
                rewardMod = 0.7f;
                casualtyMod = 0.6f;
                break;
            }

            int modifiedStrength = army->getTotalStrength() * strengthMod;
            int modifiedGoldReward = goldReward * rewardMod;
            int modifiedResourceReward = resourceReward * rewardMod;
            int modifiedCasualtyRisk = casualtyRisk * casualtyMod;

            cout << "\nFinal battle plan:" << endl;
            cout << "Effective army strength: " << modifiedStrength << endl;
            cout << "Potential reward: " << modifiedGoldReward << " gold, "
                << modifiedResourceReward << " resources" << endl;
            cout << "Casualty risk: " << modifiedCasualtyRisk << "%" << endl;

            cout << "\nCommence battle? (1 for yes, 0 for no): ";
            int confirm;
            cin >> confirm;
            cin.ignore(1000, '\n');

            if (confirm == 1) {
                cout << "\n=== BATTLE COMMENCING ===\n" << endl;
                cout << "Your forces " << (modifiedStrength > enemyStrength ? "OUTNUMBER" : "are OUTNUMBERED by")
                    << " the enemy." << endl;

                // Determine battle outcome
                bool victory = modifiedStrength > enemyStrength * (0.7 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.6);

                if (victory) {
                    cout << "\nVICTORY! Your forces have prevailed in " << battleName << "!" << endl;

                    // Award gold and resources
                    treasury->deposit(modifiedGoldReward);

                    // Add resources
                    if (economy->getNumResources() > 0) {
                        int resourceIndex = rand() % economy->getNumResources();
                        Resource* resource = economy->getResource(resourceIndex);
                        if (resource) {
                            resource->setAmount(resource->getAmount() + modifiedResourceReward);
                            cout << "You gained " << modifiedGoldReward << " gold and "
                                << modifiedResourceReward << " " << resource->getName() << "!" << endl;
                        }
                    }

                    // Boost morale
                    army->setMorale(min(100, army->getMorale() + 10));

                    // Still potentially lose units
                    for (int i = 0; i < army->getNumUnits(); i++) {
                        if (rand() % 100 < modifiedCasualtyRisk / 2) { // Half casualty risk for victory
                            MilitaryUnit* unit = army->getUnit(i);
                            string unitName = unit->getName();
                            army->removeUnit(i);
                            cout << "Your unit " << unitName << " was lost in battle." << endl;
                            i--; // Adjust index after removal
                        }
                    }
                }
                else {
                    cout << "\nDEFEAT! Your forces have been defeated in " << battleName << "." << endl;

                    // Reduce morale
                    army->setMorale(max(10, army->getMorale() - 20));

                    // Higher casualties
                    for (int i = 0; i < army->getNumUnits(); i++) {
                        if (rand() % 100 < modifiedCasualtyRisk) {
                            MilitaryUnit* unit = army->getUnit(i);
                            string unitName = unit->getName();
                            army->removeUnit(i);
                            cout << "Your unit " << unitName << " was lost in battle." << endl;
                            i--; // Adjust index after removal
                        }
                    }

                    cout << "You have suffered heavy losses and returned empty-handed." << endl;
                }

                // Train remaining units regardless of outcome
                for (int i = 0; i < army->getNumUnits(); i++) {
                    MilitaryUnit* unit = army->getUnit(i);
                    if (unit) {
                        unit->train();
                    }
                }

                cout << "\nThe battle is over. Your remaining forces have returned to the kingdom." << endl;
            }
            else {
                cout << "Battle plans canceled." << endl;
            }
            break;
        }
        case 6:
        default:
            exitMenu = true;
            break;
        }

        if (!exitMenu) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
}

// Population management interface
void GameEngine::populationInterface() {
    // Implementation would go here
   
    cout << "\nPress Enter to continue...";
    cin.get();
}

// Diplomacy interface
void GameEngine::diplomacyInterface() {
    // Implementation would go here
    
    cout << "\nPress Enter to continue...";
    cin.get();
}

// Handle AI decisions for non-player kingdoms
void GameEngine::handleAIDecisions() {
    for (int i = 0; i < numKingdoms; i++) {
        if (kingdoms[i] && kingdoms[i] != playerKingdom) {
            // AI kingdom logic
            Kingdom* aiKingdom = kingdoms[i];

            // Random economy decisions
            Economy* economy = aiKingdom->getEconomy();
            if (economy) {
                // Adjust production based on resources
                int productionChange = randomInt(-5, 10);
                economy->setProductionLevel(max(10, economy->getProductionLevel() + productionChange));

                // Adjust trade based on market conditions
                int tradeChange = randomInt(-5, 10);
                economy->setTradeLevel(max(10, economy->getTradeLevel() + tradeChange));

                // Random resource management
                for (int r = 0; r < economy->getNumResources(); r++) {
                    Resource* resource = economy->getResource(r);
                    if (resource) {
                        // AI might adjust gather rates based on needs
                        int gatherAdjustment = randomInt(-2, 5);
                        resource->adjustGatherRate(gatherAdjustment);
                    }
                }
            }

            // Military decisions
            Army* army = aiKingdom->getArmy();
            if (army) {
                // Randomly adjust military strategy
                int strategyChoice = randomInt(0, 3);
                army->setStrategy(static_cast<CombatStrategy>(strategyChoice));

                // Random training adjustments
                int trainingChange = randomInt(-3, 7);
                army->setTrainingLevel(max(10, army->getTrainingLevel() + trainingChange));
            }

            // Leadership decisions
            Leader* leader = aiKingdom->getCurrentLeader();
            if (leader) {
                // Leaders gain experience over time
                leader->incrementExperience();
                leader->calculateLeadershipScore();
            }

            // Population decisions
            Population* population = aiKingdom->getPopulation();
            if (population) {
                // Adjust growth rate based on resources
                int growthChange = randomInt(-1, 2);
                population->setGrowthRate(max(0, population->getGrowthRate() + growthChange));
            }

            // Decision to attack player?
            if (randomInt(1, 100) <= 10) { // 10% chance to attack player
                if (army && army->getTotalStrength() > 100) {
                    cout << aiKingdom->getName() << " is preparing for potential military action!" << endl;
                    // Actual attack would be implemented in processTurn
                }
            }
        }
    }
}

// Generate AI response to player input (chatbot functionality)
void GameEngine::generateAIResponse(const string& input, string& response) {
    // Simple keyword-based chatbot for demonstration purposes
    if (input.find("hello") != string::npos || input.find("hi") != string::npos) {
        response = "Greetings, your majesty! How may I assist you with your kingdom?";
    }
    else if (input.find("help") != string::npos) {
        response = "I can provide advice on economy, military, population, or diplomacy. What area interests you?";
    }
    else if (input.find("economy") != string::npos || input.find("gold") != string::npos || input.find("money") != string::npos) {
        response = "For a strong economy, balance tax rates carefully and invest in resource production. Would you like specific advice on taxes or trade?";
    }
    else if (input.find("military") != string::npos || input.find("army") != string::npos || input.find("war") != string::npos) {
        response = "A strong military requires proper training and maintenance. Balance offensive and defensive capabilities based on your neighbors.";
    }
    else if (input.find("population") != string::npos || input.find("people") != string::npos) {
        response = "Keep your population happy with adequate food and reasonable taxes. Watch for disease outbreaks and respond quickly.";
    }
    else if (input.find("diplomacy") != string::npos || input.find("alliance") != string::npos || input.find("treaty") != string::npos) {
        response = "Diplomatic relations are crucial. Consider alliances with stronger kingdoms and trade agreements to strengthen your position.";
    }
    else if (input.find("weather") != string::npos || input.find("disease") != string::npos) {
        response = "Environmental factors can severely impact your kingdom. Prepare reserves for harsh weather and have medical facilities ready for disease.";
    }
    else if (input.find("strategy") != string::npos || input.find("advice") != string::npos) {
        response = "A balanced approach is often best. Focus on stability, maintain a reserve of resources, and be prepared for unexpected events.";
    }
    else if (input.find("thank") != string::npos) {
        response = "You're most welcome, your majesty. I am here to serve.";
    }
    else {
        // Default responses
        string defaultResponses[] = {
            "I'm not sure I understand. Could you rephrase that?",
            "As your advisor, I'm focused on matters of state. What concerns you about the kingdom?",
            "Perhaps we should focus on the immediate challenges facing your kingdom.",
            "Your leadership is needed on more pressing matters. What kingdom issue can I help with?",
            "Let us discuss the state of your royal treasury or military instead."
        };
        response = defaultResponses[randomInt(0, 4)];
    }
}

// Process chat message between players or with AI
void GameEngine::processChatMessage(const string& message, int fromPlayerId, int toPlayerId) {
    if (!isMultiplayerMode && (fromPlayerId != 0 || toPlayerId != -1)) {
        // In single player mode, only player->AI chat is allowed
        return;
    }

    // If this is a message to the AI advisor (toPlayerId == -1)
    if (toPlayerId == -1) {
        string response;
        generateAIResponse(message, response);

        cout << "\n[Royal Advisor]: " << response << endl;
        cout << "\nPress Enter to continue...";
        cin.get();
        return;
    }

    // In multiplayer mode, relay the message to the appropriate player
    if (isMultiplayerMode) {
        if (toPlayerId == -2) { // Broadcast to all players
            cout << "\n[Kingdom " << fromPlayerId << " Announcement]: " << message << endl;
        }
        else {
            cout << "\n[Private message from Kingdom " << fromPlayerId
                << " to Kingdom " << toPlayerId << "]: " << message << endl;
        }
    }
}

// Check for game ending conditions
void GameEngine::checkGameEndingConditions() {
    if (!playerKingdom) {
        cout << "Game over: Player kingdom has been destroyed!" << endl;
        isGameRunning = false;
        return;
    }

    // Check for victory conditions (example: all other kingdoms conquered)
    bool allOthersDefeated = true;
    for (int i = 0; i < numKingdoms; i++) {
        if (kingdoms[i] && kingdoms[i] != playerKingdom) {
            allOthersDefeated = false;
            break;
        }
    }

    if (allOthersDefeated && numKingdoms > 1) {
        cout << "Victory! You have conquered all other kingdoms!" << endl;
        isGameRunning = false;
        return;
    }

    // Check for defeat conditions (example: stability too low)
    if (playerKingdom->getStabilityLevel() <= 10) {
        cout << "Your kingdom is on the brink of collapse due to low stability!" << endl;
        // This is just a warning, not game over yet
    }

    // Optional turn limit
    if (currentTurn >= 100) {
        cout << "You have reached the maximum number of turns (100)!" << endl;
        cout << "Final Score: " << calculateFinalScore() << endl;
        isGameRunning = false;
        return;
    }
}

// Calculate final score
int GameEngine::calculateFinalScore() const {
    if (!playerKingdom) return 0;

    int score = 0;

    // Population factors
    Population* population = playerKingdom->getPopulation();
    if (population) {
        score += population->getTotalPopulation() / 100;
        score += population->getHealthLevel();
        if (!population->isUnrestActive()) score += 100;
    }

    // Economic factors
    Economy* economy = playerKingdom->getEconomy();
    if (economy) {
        Treasury* treasury = economy->getTreasury();
        if (treasury) {
            score += treasury->getGold() / 100;
            if (treasury->calculateBalance() > 0) score += 100;
        }

        score += economy->getProductionLevel();
        score += economy->getTradeLevel();
        score += economy->getMarketStability();
    }

    // Military factors
    Army* army = playerKingdom->getArmy();
    if (army) {
        score += army->getTotalStrength() / 10;
        score += army->getOverallMorale();
    }

    // Leadership factors
    Leader* leader = playerKingdom->getCurrentLeader();
    if (leader) {
        score += leader->getLeadershipScore();
    }

    // Kingdom stability
    score += playerKingdom->getStabilityLevel() * 2;

    // Turn factor (earlier victory is better)
    score += max(0, 1000 - currentTurn * 5);

    return score;
}

// Save the game to a file
void GameEngine::saveGame(const string& filename) {
    string saveFilename = filename;

    if (saveFilename.empty()) {
        cout << "Enter filename to save (default: stronghold_save.txt): ";
        getline(cin, saveFilename);

        if (saveFilename.empty()) {
            saveFilename = "stronghold_save.txt";
        }
    }

    ofstream saveFile(saveFilename);
    if (!saveFile.is_open()) {
        cout << "Error: Could not open file for saving: " << saveFilename << endl;
        return;
    }

    // Save game engine state
    saveFile << isGameRunning << endl;
    saveFile << isGamePaused << endl;
    saveFile << gameSpeed << endl;
    saveFile << static_cast<int>(difficulty) << endl;
    saveFile << currentTurn << endl;

    // Save kingdoms
    saveFile << numKingdoms << endl;

    // Save player kingdom index
    int playerKingdomIndex = -1;
    for (int i = 0; i < numKingdoms; i++) {
        if (kingdoms[i] == playerKingdom) {
            playerKingdomIndex = i;
            break;
        }
    }
    saveFile << playerKingdomIndex << endl;

    // Save each kingdom
    for (int i = 0; i < numKingdoms; i++) {
        kingdoms[i]->save(saveFile);
    }

    saveFile.close();
    cout << "Game saved successfully to " << saveFilename << endl;
}

// Simplified version for menu
void GameEngine::saveGame() {
    saveGame("");
}

// New GameEngine getters and setters for multiplayer
bool GameEngine::getIsMultiplayerMode() const {
    return isMultiplayerMode;
}

void GameEngine::setIsMultiplayerMode(bool isMultiplayer) {
    isMultiplayerMode = isMultiplayer;
}

int GameEngine::getNumHumanPlayers() const {
    return numHumanPlayers;
}

void GameEngine::setNumHumanPlayers(int players) {
    if (players < 1) players = 1;
    if (players > 4) players = 4; // Maximum 4 human players
    numHumanPlayers = players;
}

bool GameEngine::getChatbotEnabled() const {
    return chatbotEnabled;
}

void GameEngine::setChatbotEnabled(bool enabled) {
    chatbotEnabled = enabled;
}

// Multiplayer setup
void GameEngine::setupMultiplayerGame(int numPlayers) {
    if (numPlayers < 2) {
        cout << "Multiplayer requires at least 2 players." << endl;
        return;
    }

    setIsMultiplayerMode(true);
    setNumHumanPlayers(numPlayers);

    cout << "=== MULTIPLAYER GAME SETUP ===\n" << endl;
    cout << "Number of human players: " << numHumanPlayers << endl;

    // Clean up existing kingdoms
    for (int i = 0; i < numKingdoms; i++) {
        delete kingdoms[i];
        kingdoms[i] = nullptr;
    }
    numKingdoms = 0;
    playerKingdom = nullptr;

    // Create kingdoms for each player
    for (int i = 0; i < numHumanPlayers; i++) {
        string kingdomName;
        string leaderName;
        string leaderTitle;

        cout << "\nPlayer " << (i + 1) << " setup:" << endl;

        cout << "Enter kingdom name: ";
        getline(cin, kingdomName);

        cout << "Enter leader name: ";
        getline(cin, leaderName);

        cout << "Enter leader title: ";
        getline(cin, leaderTitle);

        // Trim input strings
        trimString(kingdomName);
        trimString(leaderName);
        trimString(leaderTitle);

        // Default values if empty
        if (kingdomName.empty()) kingdomName = "Kingdom " + to_string(i + 1);
        if (leaderName.empty()) leaderName = "Player " + to_string(i + 1);
        if (leaderTitle.empty()) leaderTitle = "Lord";

        // Create a new kingdom with the given information
        Kingdom* newKingdom = new Kingdom(kingdomName, true);

        // Create a leader for the kingdom
        Leader* newLeader = new Leader(leaderName, leaderTitle);

        // Set random stats for the leader
        newLeader->setIntelligence(randomInt(50, 80));
        newLeader->setMilitarySkill(randomInt(50, 80));
        newLeader->setEconomicSkill(randomInt(50, 80));

        // Give the leader some random traits
        LeadershipTrait* trait1 = new LeadershipTrait("Charismatic", "Beloved by the people", true, CHARISMATIC);
        newLeader->addTrait(trait1);

        // Set the leader as the current leader of the kingdom
        newKingdom->setCurrentLeader(newLeader);

        // Add kingdom to the game engine
        addKingdom(newKingdom);

        // The first player is considered the "main" player for display purposes
        if (i == 0) {
            setPlayerKingdom(newKingdom);
        }
    }

    // Add AI kingdoms if desired
    char addAI;
    cout << "\nAdd AI kingdoms (y/n)? ";
    cin >> addAI;
    cin.ignore(); // Clear newline

    if (addAI == 'y' || addAI == 'Y') {
        int numAI;
        cout << "How many AI kingdoms? ";
        cin >> numAI;
        cin.ignore(); // Clear newline

        for (int i = 0; i < numAI; i++) {
            // Create an AI-controlled kingdom
            Kingdom* aiKingdom = new Kingdom("AI Kingdom " + to_string(i + 1), false);
            Leader* aiLeader = new Leader("AI Ruler " + to_string(i + 1), "Enemy King");
            aiLeader->setIntelligence(randomInt(60, 90));
            aiLeader->setMilitarySkill(randomInt(60, 90));
            aiLeader->setEconomicSkill(randomInt(60, 90));
            aiKingdom->setCurrentLeader(aiLeader);

            // Add kingdom to the game engine
            addKingdom(aiKingdom);
        }
    }

    // Enable chatbot
    setChatbotEnabled(true);

    cout << "\nMultiplayer game setup complete! " << numKingdoms << " kingdoms created." << endl;
    cout << "\nPress Enter to begin the game...";
    cin.get();

    startGame();
}

// Handle player turn in multiplayer
void GameEngine::processPlayerTurn(int playerId) {
    if (playerId < 0 || playerId >= numKingdoms) {
        return;
    }

    Kingdom* currentPlayerKingdom = kingdoms[playerId];
    if (!currentPlayerKingdom) {
        return;
    }

    // Store original player kingdom
    Kingdom* originalPlayerKingdom = playerKingdom;

    // Temporarily set this as the player kingdom
    setPlayerKingdom(currentPlayerKingdom);

    // Process this player's interface
    cout << "\n=== PLAYER " << (playerId + 1) << "'S TURN ===\n" << endl;
    cout << "Kingdom: " << currentPlayerKingdom->getName() << endl;

    Leader* leader = currentPlayerKingdom->getCurrentLeader();
    if (leader) {
        cout << "Leader: " << leader->getTitle() << " " << leader->getName() << endl;
    }

    userInterface();

    // Restore original player kingdom
    setPlayerKingdom(originalPlayerKingdom);
}

// Synchronize game state in multiplayer
void GameEngine::synchronizeGameState() {
    // In a real networked multiplayer game, this would send game state to all players
    // For this demo, we'll just display a message
    cout << "Synchronizing game state among all players..." << endl;
}

// Handle player chat
void GameEngine::handlePlayerChat() {
    if (!isMultiplayerMode && !chatbotEnabled) {
        return;
    }

    cout << "\n=== CHAT INTERFACE ===\n" << endl;

    if (isMultiplayerMode) {
        cout << "Available players:" << endl;
        for (int i = 0; i < numKingdoms; i++) {
            if (kingdoms[i]) {
                cout << i << ". " << kingdoms[i]->getName() << endl;
            }
        }
        cout << "-1. Royal Advisor (AI)" << endl;
        cout << "-2. Broadcast to all" << endl;
    }
    else {
        cout << "You can chat with your royal advisor for guidance." << endl;
    }

    int toPlayer = -1;
    if (isMultiplayerMode) {
        cout << "\nSend message to (enter number): ";
        cin >> toPlayer;
        cin.ignore(); // Clear newline
    }

    string message;
    cout << "Enter your message: ";
    getline(cin, message);

    if (message.empty()) {
        return;
    }

    // Process the message
    processChatMessage(message, 0, toPlayer); // Assuming player ID 0 is the local player
}

// Get AI response for chatbot
string GameEngine::getChatbotResponse(const string& message) {
    string response;
    generateAIResponse(message, response);
    return response;
}

// Load the game from a file
void GameEngine::loadGame(const string& filename) {
    ifstream loadFile(filename);
    if (!loadFile.is_open()) {
        throw runtime_error("Could not open file for loading: " + filename);
    }

    // Clean up existing kingdoms
    for (int i = 0; i < numKingdoms; i++) {
        delete kingdoms[i];
        kingdoms[i] = nullptr;
    }
    numKingdoms = 0;
    playerKingdom = nullptr;

    // Load game engine state
    loadFile >> isGameRunning;
    loadFile >> isGamePaused;
    loadFile >> gameSpeed;

    int difficultyInt;
    loadFile >> difficultyInt;
    difficulty = static_cast<GameDifficulty>(difficultyInt);

    loadFile >> currentTurn;

    // Load kingdoms
    int loadedNumKingdoms;
    loadFile >> loadedNumKingdoms;

    // Load player kingdom index
    int playerKingdomIndex;
    loadFile >> playerKingdomIndex;
    loadFile.ignore(); // Skip newline

    // Ensure we have enough space for kingdoms
    if (loadedNumKingdoms > maxKingdoms) {
        delete[] kingdoms;
        maxKingdoms = loadedNumKingdoms;
        kingdoms = new Kingdom * [maxKingdoms];

        for (int i = 0; i < maxKingdoms; i++) {
            kingdoms[i] = nullptr;
        }
    }

    // Load each kingdom
    for (int i = 0; i < loadedNumKingdoms; i++) {
        // Create a new kingdom
        kingdoms[i] = new Kingdom("", false);
        kingdoms[i]->load(loadFile);
        numKingdoms++;

        // Set player kingdom if this is the player kingdom
        if (i == playerKingdomIndex) {
            playerKingdom = kingdoms[i];
        }
    }

    loadFile.close();
    cout << "Game loaded successfully from " << filename << endl;
}