#include "Stronghold.h"
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

// Function prototypes
void displayWelcomeMessage();
void displayMainMenu();
void startNewGame(GameEngine& gameEngine);
void loadSavedGame(GameEngine& gameEngine);
void displayCredits();
void displayHelp();

int main() {
    // Seed random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create game engine
    GameEngine gameEngine;

    bool exitGame = false;
    int choice;

    displayWelcomeMessage();

    while (!exitGame) {
        displayMainMenu();

        // Get user input
        cout << "Enter your choice: ";
        cin >> choice;

        // Clear input buffer
        cin.clear();
        cin.ignore(1000, '\n');

        // Process choice
        switch (choice) {
        case 1: // Start new game
            startNewGame(gameEngine);
            break;
        case 2: // Load saved game
            loadSavedGame(gameEngine);
            break;
        case 3: // Display help
            displayHelp();
            break;
        case 4: // Display credits
            displayCredits();
            break;
        case 5: // Exit game
            cout << "Thank you for playing StrongHold. Goodbye!" << endl;
            exitGame = true;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    }

    return 0;
}

void displayWelcomeMessage() {
    cout << "=================================================" << endl;
    cout << "              WELCOME TO STRONGHOLD               " << endl;
    cout << "           Medieval Kingdom Simulator             " << endl;
    cout << "=================================================" << endl;
    cout << "In this game, you will manage your own medieval   " << endl;
    cout << "kingdom, dealing with population dynamics, economy," << endl;
    cout << "military, politics, and random events.            " << endl;
    cout << "Your decisions will shape the fate of your realm. " << endl;
    cout << "=================================================" << endl;
    cout << endl;
}

void displayMainMenu() {
    cout << "=== MAIN MENU ===" << endl;
    cout << "1. Start New Game (Single Player)" << endl;
    cout << "2. Start Multiplayer Game" << endl;
    cout << "3. Chat with Royal Advisor" << endl;
    cout << "4. Load Saved Game" << endl;
    cout << "5. Help" << endl;
    cout << "6. Credits" << endl;
    cout << "7. Exit" << endl;
    cout << "================" << endl;
}

void startNewGame(GameEngine& gameEngine) {
    string kingdomName;
    string leaderName;
    string leaderTitle;

    cout << "=== CREATING NEW KINGDOM ===" << endl;

    cout << "Enter your kingdom's name: ";
    getline(cin, kingdomName);

    cout << "Enter your leader's name: ";
    getline(cin, leaderName);

    cout << "Enter your leader's title: ";
    getline(cin, leaderTitle);

    // Trim input strings
    trimString(kingdomName);
    trimString(leaderName);
    trimString(leaderTitle);

    // Default values if empty
    if (kingdomName.empty()) kingdomName = "New Kingdom";
    if (leaderName.empty()) leaderName = "Ruler";
    if (leaderTitle.empty()) leaderTitle = "Lord";

    // Create a new kingdom with the given information
    Kingdom* playerKingdom = new Kingdom(kingdomName, true);

    // Create a leader for the player's kingdom
    Leader* playerLeader = new Leader(leaderName, leaderTitle);

    // Set random stats for the leader
    playerLeader->setIntelligence(randomInt(50, 80));
    playerLeader->setMilitarySkill(randomInt(50, 80));
    playerLeader->setEconomicSkill(randomInt(50, 80));

    // Give the leader some random traits
    LeadershipTrait* trait1 = new LeadershipTrait("Charismatic", "Beloved by the people", true, CHARISMATIC);
    playerLeader->addTrait(trait1);

    // Set the leader as the current leader of the kingdom
    playerKingdom->setCurrentLeader(playerLeader);

    // Add an AI-controlled kingdom for competition
    Kingdom* aiKingdom = new Kingdom("Rival Kingdom", false);
    Leader* aiLeader = new Leader("AI Ruler", "Enemy King");
    aiLeader->setIntelligence(randomInt(60, 90));
    aiLeader->setMilitarySkill(randomInt(60, 90));
    aiLeader->setEconomicSkill(randomInt(60, 90));
    aiKingdom->setCurrentLeader(aiLeader);

    // Add kingdoms to the game engine
    gameEngine.addKingdom(playerKingdom);
    gameEngine.addKingdom(aiKingdom);

    // Set player kingdom
    gameEngine.setPlayerKingdom(playerKingdom);

    // Start the game
    cout << "\nYour kingdom '" << kingdomName << "' has been established!" << endl;
    cout << "You, " << leaderTitle << " " << leaderName << ", must now lead your people to prosperity." << endl;
    cout << "\nPress Enter to begin your reign..." << endl;
    cin.get();

    gameEngine.startGame();
}

void loadSavedGame(GameEngine& gameEngine) {
    string filename;

    cout << "Enter the saved game filename (default: stronghold_save.txt): ";
    getline(cin, filename);

    if (filename.empty()) {
        filename = "stronghold_save.txt";
    }

    try {
        gameEngine.loadGame(filename);
        cout << "Game loaded successfully." << endl;
        cout << "Press Enter to continue your reign..." << endl;
        cin.get();
        gameEngine.setIsGameRunning(true);
        while (gameEngine.getIsGameRunning()) {
            gameEngine.processTurn();
        }
    }
    catch (const exception& e) {
        cout << "Error loading game: " << e.what() << endl;
        cout << "Press Enter to return to main menu..." << endl;
        cin.get();
    }
}

void displayHelp() {
    cout << "=== STRONGHOLD HELP ===" << endl;
    cout << "Stronghold is a turn-based kingdom management simulation." << endl;
    cout << "\nKEY CONCEPTS:" << endl;
    cout << "1. Population: Your kingdom's inhabitants are divided into social classes." << endl;
    cout << "   Each class has different needs and contributes differently to your economy." << endl;
    cout << "   Keep them happy and healthy to prevent unrest." << endl << endl;

    cout << "2. Resources: Manage resources like food, wood, stone, gold, and iron." << endl;
    cout << "   These are gathered over time and consumed by your population and activities." << endl << endl;

    cout << "3. Economy: Set tax rates, develop trade, and manage your treasury." << endl;
    cout << "   Balance your income and expenses to maintain a healthy economy." << endl << endl;

    cout << "4. Military: Train and maintain an army to defend your kingdom." << endl;
    cout << "   Different units have different strengths and costs." << endl << endl;

    cout << "5. Leadership: Your leader's traits and skills affect all aspects of the kingdom." << endl;
    cout << "   Leadership changes can occur through elections, coups, or death." << endl << endl;

    cout << "6. Events: Random events like weather, disease, or other occurrences will challenge you." << endl;
    cout << "   Respond wisely to minimize negative impacts and capitalize on opportunities." << endl << endl;

    cout << "CONTROLS:" << endl;
    cout << "- Follow on-screen prompts to make decisions and manage your kingdom." << endl;
    cout << "- Save your game regularly to prevent loss of progress." << endl;
    cout << "- Each turn represents a period of time in your kingdom's history." << endl << endl;

    cout << "Press Enter to return to the main menu..." << endl;
    cin.get();
}

void displayCredits() {
    cout << "=== STRONGHOLD CREDITS ===" << endl;
    cout << "Developed as part of OOP Lab Final Project - Spring 2025" << endl;
    cout << "\nDevelopment Team:" << endl;
    cout << "- [Your Name]" << endl;
    cout << "- [Team Member Name]" << endl;
    cout << "\nSpecial thanks to the OOP Lab instructors and teaching assistants." << endl;
    cout << "\nPress Enter to return to the main menu..." << endl;
    cin.get();
}
