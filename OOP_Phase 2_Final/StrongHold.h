#pragma once
#ifndef STRONGHOLD_H
#define STRONGHOLD_H
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <iomanip>
using namespace std;

// Forward declarations
class Resource;
class Treasury;
class Leader;
class MilitaryUnit;
class Event;
class Kingdom;
class Weather;
class Disease;
class Battle;
class LeadershipSystem;
class Population;
class SocialClass;
class Economy;
class Bank;
class Army;
class GameEngine;

// Enumerations for game systems
enum CombatStrategy { AGGRESSIVE, DEFENSIVE, BALANCED, GUERRILLA };
enum TerrainType { PLAINS, FOREST, MOUNTAINS, DESERT, SWAMP };
enum WeatherType { SUNNY, RAINY, STORMY, SNOWY, DROUGHT, FOGGY };
enum DiseaseType { NONE, COMMON_COLD, DYSENTERY, PLAGUE, FEVER };
enum LeadershipTraitType {
    INSPIRING,    // Boosts morale
    STRATEGIC,    // Improves battle outcomes
    RUTHLESS,     // Increases strength but reduces morale
    DIPLOMATIC,   // Better at negotiations
    CORRUPT,      // Skims resources
    CHARISMATIC,  // Popularity with troops and populace
    EXPERIENCED   // Better training outcomes
};

enum EventType {
    EVENT_POSITIVE,   // Good for the kingdom
    EVENT_NEGATIVE,   // Bad for the kingdom
    EVENT_NEUTRAL     // Neutral impact
};

enum GameDifficulty {
    EASY,      // More resources, less challenges
    NORMAL,    // Balanced gameplay
    HARD,      // Fewer resources, more challenges
    HARDCORE   // Very challenging gameplay
};

enum ResourceType {
    FOOD,
    WOOD,
    STONE,
    GOLD,
    IRON
};

// Global Functions
int randomInt(int min, int max);
double randomDouble(double min, double max);
string currentDateTime();
void trimString(string& str);

// Base Entity class for common attributes and methods
class Entity
{
protected:
    string name;
    string description;
    bool isActive;
public:
    Entity(const string& name, const string& description = "");
    virtual ~Entity();

    string getName() const;
    void setName(const string& newName);
    string getDescription() const;
    void setDescription(const string& newDescription);
    bool getIsActive() const;
    void setIsActive(bool active);

    virtual void update() = 0;
    virtual void save(ofstream& file) const;
    virtual void load(ifstream& file);
};

// Resource class for handling different types of resources
class Resource : public Entity
{
private:
    ResourceType type;
    int amount;
    int gatherRate;
    int consumptionRate;
    int value;
    bool isStockpiled;
public:
    Resource(const string& name, ResourceType type = FOOD, int amount = 0, int gatherRate = 0, int consumptionRate = 0);
    ~Resource();

    ResourceType getType() const;
    int getAmount() const;
    void setAmount(int newAmount);
    void gather();
    bool consume(int amount);
    void adjustGatherRate(int rate);
    void adjustConsumptionRate(int rate);
    int getGatherRate() const;
    void setGatherRate(int rate);
    int getConsumptionRate() const;
    void setConsumptionRate(int rate);
    int getValue() const;
    void setValue(int newValue);
    bool getIsStockpiled() const;
    void setIsStockpiled(bool stockpiled);

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Base Human class for population units
class Human : public Entity
{
protected:
    int happiness;
    int health;
    int loyaltyLevel;
public:
    Human(const string& name, int happiness = 50, int health = 100, int loyaltyLevel = 50);
    virtual ~Human();

    int getHappiness() const;
    void adjustHappiness(int change);
    int getHealth() const;
    void adjustHealth(int change);
    int getLoyaltyLevel() const;
    void adjustLoyalty(int change);

    virtual bool isUnrestLikely() const;
    virtual void applyDiseaseEffect(const Disease& disease);
    virtual void applyWeatherEffect(const Weather& weather);

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Social class representation
class SocialClass : public Human
{
private:
    int population;
    double taxRate;
    int diseaseResistance;
public:
    SocialClass(const string& name, int initialPopulation, double initialTaxRate);
    ~SocialClass();

    int getPopulation() const;
    void setPopulation(int newPopulation);
    double getTaxRate() const;
    void setTaxRate(double newRate);
    int calculateTaxRevenue() const;
    int getDiseaseResistance() const;
    void setDiseaseResistance(int resistance);

    virtual bool isUnrestLikely() const override;
    virtual void applyDiseaseEffect(const Disease& disease) override;
    virtual void applyWeatherEffect(const Weather& weather) override;

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Environmental Effect base class for weather and disease
class EnvironmentalEffect : public Entity
{
protected:
    int severity;
    int duration;
    int turnsActive;
public:
    EnvironmentalEffect(const string& name, const string& description, int severity, int duration);
    virtual ~EnvironmentalEffect();

    int getSeverity() const;
    void setSeverity(int newSeverity);
    int getDuration() const;
    void setDuration(int newDuration);
    int getTurnsActive() const;
    void incrementTurnsActive();
    int getTurnsRemaining() const;

    virtual void applyEffects(Kingdom* kingdom) = 0;
    virtual void update() override = 0;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Disease class for handling epidemics and health issues
class Disease : public EnvironmentalEffect
{
private:
    DiseaseType type;
    int infectivity;
    int mortalityRate;
    int currentInfected;

public:
    Disease(const string& name, const string& description, int severity, int infectivity, int duration, DiseaseType type = NONE);
    ~Disease();

    DiseaseType getType() const;
    void setType(DiseaseType newType);
    int getInfectivity() const;
    int getMortalityRate() const;
    int getCurrentInfected() const;
    void setCurrentInfected(int infected);

    void outbreak(int initialInfected);
    void spread(Population* population, Weather* currentWeather);
    int calculateDeaths();

    virtual void applyEffects(Kingdom* kingdom) override;
    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Weather system implementation
class Weather : public EnvironmentalEffect
{
private:
    WeatherType type;
    int cropEffect;         // Effect on crop production (%)
    int movementEffect;     // Effect on movement/travel speed (%)
    int moraleEffect;       // Effect on population and army morale
    int diseaseModifier;    // How weather affects disease spread
    bool isExtreme;         // Is this extreme weather (storm, etc.)

public:
    Weather(const string& name = "Clear", const string& description = "Clear skies", int severity = 1, WeatherType type = SUNNY);
    ~Weather();

    WeatherType getType() const;
    void setType(WeatherType newType);
    int getCropEffect() const;
    void setCropEffect(int effect);
    int getMovementEffect() const;
    void setMovementEffect(int effect);
    int getMoraleEffect() const;
    void setMoraleEffect(int effect);
    int getDiseaseModifier() const;
    void setDiseaseModifier(int modifier);
    bool getIsExtreme() const;
    void setIsExtreme(bool extreme);

    void generateRandomWeather();

    virtual void applyEffects(Kingdom* kingdom) override;
    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Population management
class Population
{
private:
    int totalPopulation;
    int growthRate;
    int foodConsumptionPerCapita;
    int healthLevel;
    bool unrestActive;
    SocialClass** classes;
    int numClasses;
    int maxClasses;
    int diseaseSusceptibility;

public:
    Population(int initialPopulation = 1000);
    ~Population();

    void addSocialClass(SocialClass* newClass);
    void removeClassAt(int index);
    int getTotalPopulation() const;
    int getGrowthRate() const;
    void setGrowthRate(int rate);
    bool isUnrestActive() const;
    int getHealthLevel() const;
    void setHealthLevel(int level);
    int getDiseaseSusceptibility() const;
    void setDiseaseSusceptibility(int susceptibility);
    SocialClass* getSocialClass(int index) const;
    int getNumClasses() const;
    int getFoodConsumptionPerCapita() const;
    void setFoodConsumptionPerCapita(int consumption);
    void recalculateTotalPopulation();

    void update(int availableFood);
    void handleEvent(const Event& event);
    void handleWeatherEffects(const Weather& weather);
    void handleDiseaseEffects(const Disease& disease);
    void save(ofstream& file) const;
    void load(ifstream& file);
};

// Leadership trait class
class LeadershipTrait : public Entity
{
private:
    LeadershipTraitType traitType;
    int charismaEffect;
    int intelligenceEffect;
    int militaryEffect;
    int economicEffect;
    int corruptionEffect;
    bool isPositive;

public:
    LeadershipTrait(const string& name, const string& description, bool isPositive, LeadershipTraitType type = INSPIRING);
    ~LeadershipTrait();

    LeadershipTraitType getTraitType() const;
    void setTraitType(LeadershipTraitType type);
    int getCharismaEffect() const;
    void setCharismaEffect(int effect);
    int getIntelligenceEffect() const;
    void setIntelligenceEffect(int effect);
    int getMilitaryEffect() const;
    void setMilitaryEffect(int effect);
    int getEconomicEffect() const;
    void setEconomicEffect(int effect);
    int getCorruptionEffect() const;
    void setCorruptionEffect(int effect);
    bool getIsPositive() const;

    void applyEffects(Leader* leader);
    void removeEffects(Leader* leader);

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Leader class
class Leader : public Human
{
private:
    string title;
    int intelligence;
    int militarySkill;
    int economicSkill;
    int corruption;
    int leadershipScore;
    int experience;
    LeadershipTrait** traits;
    int numTraits;
    int maxTraits;
    int termLength;
    bool isElected;

public:
    Leader(const string& name, const string& title);
    ~Leader();

    string getTitle() const;
    void setTitle(const string& newTitle);
    int getIntelligence() const;
    void setIntelligence(int value);
    int getMilitarySkill() const;
    void setMilitarySkill(int value);
    int getEconomicSkill() const;
    void setEconomicSkill(int value);
    int getCorruption() const;
    void setCorruption(int level);
    int getLeadershipScore() const;
    void calculateLeadershipScore();
    int getExperience() const;
    void incrementExperience();
    int getTermLength() const;
    void incrementTermLength();
    bool getIsElected() const;
    void setIsElected(bool elected);

    void addTrait(LeadershipTrait* trait);
    void removeTrait(int index);
    LeadershipTrait* getTrait(int index) const;
    int getNumTraits() const;

    double calculatePopulationBonus() const;
    double calculateMilitaryBonus() const;
    double calculateEconomicBonus() const;
    double calculateDiseasePrevention() const;
    double calculateWeatherPreparedness() const;
    bool makeDecision(int difficulty);

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Leadership system for managing succession and leadership changes
class LeadershipSystem
{
private:
    Leader** potentialLeaders;
    int numLeaders;
    int maxLeaders;
    int electionCycle;
    int turnsToNextElection;
    int stabilityFactor;
    int coupRisk;
    Kingdom* kingdom;

public:
    LeadershipSystem(Kingdom* kingdom);
    ~LeadershipSystem();

    void addPotentialLeader(Leader* leader);
    void removePotentialLeader(int index);
    Leader* getPotentialLeader(int index) const;
    int getNumPotentialLeaders() const;

    int getElectionCycle() const;
    void setElectionCycle(int cycles);
    int getTurnsToNextElection() const;
    int getStabilityFactor() const;
    void setStabilityFactor(int stability);
    int getCoupRisk() const;
    void setCoupRisk(int risk);

    Leader* generateRandomLeader();
    void holdElection();
    bool checkForCoup();
    void handleCoup();
    void handleDeath();
    void handleSuccession();

    void update();
    void save(ofstream& file) const;
    void load(ifstream& file);
};

// Military unit base class
class CombatUnit : public Entity
{
protected:
    int attack;
    int defense;
    int speed;

public:
    CombatUnit(const string& name, const string& description, int attack = 10, int defense = 5, int speed = 5);
    virtual ~CombatUnit();

    int getAttack() const;
    void setAttack(int value);
    int getDefense() const;
    void setDefense(int value);
    int getSpeed() const;
    void setSpeed(int value);

    virtual int calculatePower() const;
    virtual void train();
    virtual void takeDamage(int damage);
    virtual int dealDamage(CombatStrategy strategy);
    virtual void applyWeatherEffects(const Weather& weather);
    virtual void applyLeaderEffects(const Leader& leader);

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Specific military unit types
class MilitaryUnit : public CombatUnit
{
private:
    int count;
    int maxCount;
    int morale;
    int experience;
    int foodConsumption;
    int trainingLevel;
    bool isVeteran;

public:
    MilitaryUnit(const string& name, int count = 0, int attack = 10, int defense = 5, int speed = 5, int foodConsumption = 1);
    ~MilitaryUnit();

    int getCount() const;
    void setCount(int newCount);
    int getMaxCount() const;
    void setMaxCount(int newMax);
    int getMorale() const;
    void adjustMorale(int change);
    int getExperience() const;
    void gainExperience(int amount);
    int getFoodConsumption() const;
    void setFoodConsumption(int amount);
    int getTrainingLevel() const;
    bool getIsVeteran() const;
    int getCombatStrength() const;
    void takeCasualties(int casualties);
    void recruit(int newTroops);

    virtual int calculatePower() const override;
    virtual void train() override;
    int calculateUpkeepCost() const;
    double calculateTerrainModifier(TerrainType battleTerrain) const;

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Army class for managing military units
class Army : public Entity
{
private:
    MilitaryUnit** units;
    int numUnits;
    int maxUnits;
    int totalStrength;
    int overallMorale;
    int discipline;
    int trainingLevel;
    int foodConsumption;
    CombatStrategy strategy;
    int lastBattleResult;

public:
    Army(const string& name = "Royal Army");
    ~Army();

    void addUnit(MilitaryUnit* unit);
    void removeUnit(int index);
    MilitaryUnit* getUnit(int index) const;
    int getNumUnits() const;
    int getTotalStrength() const;
    int getOverallMorale() const;
    int getMorale() const;
    void setMorale(int morale);
    int getDiscipline() const;
    void setDiscipline(int discipline);
    int getTrainingLevel() const;
    void setTrainingLevel(int level);
    int getFoodConsumption() const;
    CombatStrategy getStrategy() const;
    void setStrategy(CombatStrategy newStrategy);
    int getLastBattleResult() const;
    void setLastBattleResult(int result);

    void trainAllUnits();
    void adjustMorale(int amount);
    void train();
    void recruit(MilitaryUnit* newUnit);
    void calculateTotalStrength();
    void calculateOverallMorale();
    void calculateFoodConsumption();
    int calculateMaintenanceCost() const;
    int getMaintenanceCost() const;
    int calculateAttackPower() const;
    int calculateDefensePower() const;
    void applyWeatherEffects(const Weather& weather);
    void applyLeaderEffects(const Leader& leader);
    void handleBattleResults(bool victory, int casualties);
    void processBattleResults(bool victory, int casualtyPercentage);

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Treasury class for financial management
class Treasury : public Entity
{
private:
    int gold;
    int income;
    int expenses;
    int taxIncome;
    int tradeIncome;
    int otherIncome;
    int militaryExpenses;
    int buildingExpenses;
    int otherExpenses;
    int corruption;
    double inflation;

public:
    Treasury(const string& name);
    ~Treasury();

    int getGold() const;
    void setGold(int amount);
    int getIncome() const;
    void setIncome(int amount);
    int getExpenses() const;
    void setExpenses(int amount);
    int getTaxIncome() const;
    void setTaxIncome(int amount);
    int getTradeIncome() const;
    void setTradeIncome(int amount);
    int getOtherIncome() const;
    void setOtherIncome(int amount);
    int getMilitaryExpenses() const;
    void setMilitaryExpenses(int amount);
    int getBuildingExpenses() const;
    void setBuildingExpenses(int amount);
    int getOtherExpenses() const;
    void setOtherExpenses(int amount);
    int getCorruption() const;
    void setCorruption(int level);
    double getInflation() const;
    void setInflation(double rate);

    void recalculateTotalIncome();
    void recalculateExpenses();
    int calculateBalance() const;

    bool spend(int amount);
    void earn(int amount);
    void deposit(int amount);
    void calculateTaxIncome(const Population* population);
    void calculateTradeIncome(int tradeLevel, int numResources);
    void calculateMilitaryExpenses(const Army* army);
    void applyTurnChanges();

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Economy class for economic simulation
// Structure for trade routes
struct TradeRoute
{
    Kingdom* targetKingdom;
    string targetKingdomName;  // Used when targetKingdom is nullptr
    ResourceType exportResource;
    int exportAmount;
    ResourceType importResource;
    int importAmount;
    double profitMargin;
    bool isActive;

    TradeRoute() : targetKingdom(nullptr), targetKingdomName(""), exportResource(FOOD),
        exportAmount(0), importResource(FOOD),
        importAmount(0), profitMargin(0.0), isActive(false) {
    }
};

class Economy : public Entity
{
private:
    Treasury* treasury;
    int productionLevel;
    int tradeLevel;
    double taxRate;
    double tariffRate;  // Added tariff rate for international trade
    int marketStability;
    double inflation;
    int employmentRate;
    int corruptionLevel;
    Resource** resources;
    int numResources;
    int maxResources;
    Bank* bank;

    // Trade routes
    TradeRoute** tradeRoutes;
    int numTradeRoutes;
    int maxTradeRoutes;

public:
    Economy(const string& name, Treasury* treasury);
    ~Economy();

    Treasury* getTreasury() const;
    Bank* getBank() const;
    void setBank(Bank* newBank);
    int getProductionLevel() const;
    void setProductionLevel(int level);
    int getTradeLevel() const;
    void setTradeLevel(int level);
    double getTaxRate() const;
    void setTaxRate(double rate);
    double getTariffRate() const;
    void setTariffRate(double rate);
    int getMarketStability() const;
    void setMarketStability(int stability);
    double getInflation() const;
    void setInflation(double rate);
    int getEmploymentRate() const;
    void setEmploymentRate(int rate);
    int getCorruptionLevel() const;
    void setCorruptionLevel(int level);

    void addResource(Resource* resource);
    void removeResource(int index);
    Resource* getResource(int index) const;
    Resource* getResourceByType(ResourceType type) const;
    int getNumResources() const;

    // Trade route methods
    void addTradeRoute(Kingdom* targetKingdom, ResourceType exportResource, int exportAmount,
        ResourceType importResource, int importAmount);
    void addTradeRoute(const string& targetKingdomName, ResourceType exportResource, int exportAmount,
        ResourceType importResource, int importAmount);
    void removeTradeRoute(int index);
    int getNumTradeRoutes() const;
    int getMaxTradeRoutes() const;
    void displayTradeRoutes() const;
    void processTradeRoutes();

    void collectTaxes(Population* population);
    void payArmy(Army* army);
    void adjustPrices();
    void handleCorruption(const Leader& leader);
    void handleTradeEffects();
    void handleWeatherEffects(const Weather& weather);
    bool trade(ResourceType sellResource, int sellAmount, ResourceType buyResource, int buyAmount);
    int calculateProductionOutput() const;
    void calculateInflation();

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Loan structure for banking system
struct Loan
{
    string name;
    int amount;
    double interestRate;
    int duration;
    int turnsRemaining;
    bool isActive;

    Loan(const string& loanName)
        : name(loanName), amount(0), interestRate(0),
        duration(0), turnsRemaining(0), isActive(false) {
    }
};

// Investment class
class Investment
{
public:
    string name;
    int amount;
    double returnRate;
    int duration;
    int turnsRemaining;
    bool isActive;

    Investment(const string& investmentName)
        : name(investmentName), amount(0), returnRate(0),
        duration(0), turnsRemaining(0), isActive(false) {
    }
};

// Banking system
class Bank : public Entity
{
private:
    double interestRate;
    double investmentReturnRate;
    int goldReserves;
    int totalLoans;
    int loanLimit;
    int defaultRisk;
    int fraudLevel;
    Loan** loans;
    int numLoans;
    int maxLoans;
    Investment** investments;
    int numInvestments;
    int maxInvestments;

public:
    Bank(const string& name);
    ~Bank();

    double getInterestRate() const;
    void setInterestRate(double rate);
    double getInvestmentReturnRate() const;
    void setInvestmentReturnRate(double rate);
    int getReserve() const;
    int getGoldReserves() const;
    void setGoldReserves(int amount);
    int getTotalLoans() const;
    int getLoanLimit() const;
    void setLoanLimit(int limit);
    int getDefaultRisk() const;
    void setDefaultRisk(int risk);
    int getFraudLevel() const;
    void setFraudLevel(int level);
    int getNumLoans() const;
    int getNumInvestments() const;

    bool takeLoan(int amount, int duration);
    void repayLoan(int amount);
    void adjustInterestRate(int economicStability);
    void handleFraud(const Leader& leader);
    void audit();
    void processDefaults();

    bool provideLoan(int amount, int duration, Treasury* treasury);
    int repayLoans(int amount, Treasury* treasury);
    int displayLoans();
    void displayInvestments();
    bool makeInvestment(int amount, int duration, Treasury* treasury);

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Event class for random occurrences
class Event : public Entity
{
private:
    EventType type;
    int populationEffect;
    int economyEffect;
    int militaryEffect;
    int resourceEffect;
    int duration;
    int turnsRemaining;
    ResourceType affectedResourceType;

public:
    Event(const string& name, const string& description, EventType type = EVENT_NEUTRAL);
    ~Event();

    EventType getType() const;
    void setType(EventType newType);
    int getPopulationEffect() const;
    void setPopulationEffect(int effect);
    int getEconomyEffect() const;
    void setEconomyEffect(int effect);
    int getMilitaryEffect() const;
    void setMilitaryEffect(int effect);
    int getResourceEffect() const;
    void setResourceEffect(int effect);
    int getDuration() const;
    void setDuration(int newDuration);
    int getTurnsRemaining() const;
    void decrementTurnsRemaining();
    ResourceType getAffectedResourceType() const;
    void setAffectedResourceType(ResourceType type);

    static Event* generateRandomEvent();
    void applyEffects(Kingdom* kingdom);

    virtual void update() override;
    virtual void save(ofstream& file) const override;
    virtual void load(ifstream& file) override;
};

// Kingdom class for managing overall game state
class Kingdom
{
private:
    string name;
    Population* population;
    Economy* economy;
    Army* army;
    Leader* currentLeader;
    LeadershipSystem* leadershipSystem;
    Weather* currentWeather;
    Disease* currentDisease;
    Event** activeEvents;
    int numEvents;
    int maxEvents;
    int turn;
    bool isPlayerControlled;
    Bank* bank;
    int stabilityLevel;

public:
    Kingdom(const string& name, bool isPlayerControlled = true);
    ~Kingdom();

    string getName() const;
    void setName(const string& newName);
    Population* getPopulation() const;
    Economy* getEconomy() const;
    Army* getArmy() const;
    Leader* getCurrentLeader() const;
    void setCurrentLeader(Leader* leader);
    LeadershipSystem* getLeadershipSystem() const;
    Weather* getCurrentWeather() const;
    void setCurrentWeather(Weather* weather);
    Disease* getCurrentDisease() const;
    void setCurrentDisease(Disease* disease);
    int getTurn() const;
    void incrementTurn();
    bool getIsPlayerControlled() const;
    void setIsPlayerControlled(bool isPlayer);
    Bank* getBank() const;
    int getStabilityLevel() const;
    void setStabilityLevel(int level);

    void addEvent(Event* event);
    void removeEvent(int index);
    Event* getEvent(int index) const;
    int getNumEvents() const;

    void processTurn();
    void calculateStability();

    void updateResources();
    void updatePopulation();
    void updateEconomy();
    void updateArmy();
    void updateLeadership();
    void updateWeather();
    void updateDisease();
    void updateEvents();
    void updateStability();
    void generateRandomEvents();

    void update();
    void save(ofstream& file) const;
    void save(const string& filename) const;
    void load(ifstream& file);
    void load(const string& filename);
};

// Game Engine for managing the game
class GameEngine
{
private:
    Kingdom** kingdoms;
    int numKingdoms;
    int maxKingdoms;
    int currentTurn;
    bool isGameRunning;
    bool isGamePaused;
    int gameSpeed;
    GameDifficulty difficulty;
    Kingdom* playerKingdom;
    bool isMultiplayerMode;
    int numHumanPlayers;
    bool chatbotEnabled;

public:
    GameEngine();
    ~GameEngine();

    void addKingdom(Kingdom* kingdom);
    void removeKingdom(int index);
    Kingdom* getKingdom(int index) const;
    int getNumKingdoms() const;
    int getCurrentTurn() const;
    void setCurrentTurn(int turn);
    bool getIsGameRunning() const;
    void setIsGameRunning(bool running);
    bool getIsGamePaused() const;
    void setIsGamePaused(bool paused);
    int getGameSpeed() const;
    void setGameSpeed(int speed);
    GameDifficulty getDifficulty() const;
    void setDifficulty(GameDifficulty diff);
    Kingdom* getPlayerKingdom() const;
    void setPlayerKingdom(Kingdom* kingdom);
    bool getIsMultiplayerMode() const;
    void setIsMultiplayerMode(bool isMultiplayer);
    int getNumHumanPlayers() const;
    void setNumHumanPlayers(int players);
    bool getChatbotEnabled() const;
    void setChatbotEnabled(bool enabled);

    // Game Flow Methods
    void startGame();
    void endGame();
    void processTurn();
    void displayGameState() const;
    void userInterface();
    void displayDetailedKingdomInfo() const;

    // Economy Management Methods
    void economyInterface();
    void adjustTaxRates();
    void manageResources();
    void bankingInterface();
    void manageTradeOptions();
    void viewEconomicReport();

    // Military Management Methods
    void militaryInterface();
    void recruitUnits();
    void trainArmy();
    void manageMilitaryStrategy();
    void viewMilitaryReport();
    void planAttack();
    void defendKingdom();

    // Population Management Methods
    void populationInterface();
    void managePopulationHealth();
    void handlePopulationUnrest();
    void adjustFoodDistribution();
    void viewPopulationReport();

    // Diplomacy Methods
    void diplomacyInterface();
    void negotiateAlliance();
    void declareWar();
    void establishTradeDeal();
    void manageRelationships();
    void viewDiplomacyReport();

    // AI and Game Logic Methods
    void handleAIDecisions();
    void generateAIResponse(const string& input, string& response);
    void processChatMessage(const string& message, int fromPlayerId, int toPlayerId);
    void checkGameEndingConditions();
    int calculateFinalScore() const;

    // Multiplayer Methods
    void setupMultiplayerGame(int numPlayers);
    void processPlayerTurn(int playerId);
    void synchronizeGameState();
    void handlePlayerChat();
    string getChatbotResponse(const string& message);

    // Save/Load Methods
    void saveGame(const string& filename);
    void saveGame();
    void loadGame(const string& filename);
};

// Template class for managing collections
template <class T>
class Collection
{
private:
    T** items;
    int count;
    int capacity;

public:
    Collection(int initialCapacity = 10) {
        capacity = initialCapacity;
        count = 0;
        items = new T * [capacity];
        for (int i = 0; i < capacity; i++) {
            items[i] = nullptr;
        }
    }

    ~Collection() {
        for (int i = 0; i < count; i++) {
            delete items[i];
        }
        delete[] items;
    }

    void add(T* item) {
        if (count >= capacity) {
            // Resize the array
            capacity *= 2;
            T** newItems = new T * [capacity];
            for (int i = 0; i < count; i++) {
                newItems[i] = items[i];
            }
            delete[] items;
            items = newItems;
        }
        items[count++] = item;
    }

    void remove(int index) {
        if (index < 0 || index >= count) {
            throw out_of_range("Index out of range");
        }
        delete items[index];
        for (int i = index; i < count - 1; i++) {
            items[i] = items[i + 1];
        }
        items[count - 1] = nullptr;
        count--;
    }

    T* get(int index) const {
        if (index < 0 || index >= count) {
            throw out_of_range("Index out of range");
        }
        return items[index];
    }

    int size() const {
        return count;
    }

    int getCapacity() const {
        return capacity;
    }

    void clear() {
        for (int i = 0; i < count; i++) {
            delete items[i];
            items[i] = nullptr;
        }
        count = 0;
    }
};

#endif // STRONGHOLD_H
