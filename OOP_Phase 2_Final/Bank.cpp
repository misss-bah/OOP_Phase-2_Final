#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Constructor
Bank::Bank(const string& name)
    : Entity(name, "Kingdom banking system"), interestRate(0.05), investmentReturnRate(0.03), goldReserves(1000),
    totalLoans(0), loanLimit(10000), defaultRisk(5), fraudLevel(0), maxLoans(10),
    numLoans(0), maxInvestments(10), numInvestments(0) {

    // Initialize loans array
    loans = new Loan * [maxLoans];
    for (int i = 0; i < maxLoans; i++) {
        loans[i] = nullptr;
    }

    // Initialize investments array
    investments = new Investment * [maxInvestments];
    for (int i = 0; i < maxInvestments; i++) {
        investments[i] = nullptr;
    }
}

// Destructor
Bank::~Bank() {
    // Clean up loans
    for (int i = 0; i < numLoans; i++) {
        delete loans[i];
    }
    delete[] loans;

    // Clean up investments
    for (int i = 0; i < numInvestments; i++) {
        delete investments[i];
    }
    delete[] investments;
}

// Getters and setters
double Bank::getInterestRate() const {
    return interestRate;
}

void Bank::setInterestRate(double rate) {
    if (rate < 0.01) rate = 0.01;  // 1%
    if (rate > 0.5) rate = 0.5;    // 50%
    interestRate = rate;
}

double Bank::getInvestmentReturnRate() const {
    return investmentReturnRate;
}

void Bank::setInvestmentReturnRate(double rate) {
    if (rate < 0.01) rate = 0.01;  // 1%
    if (rate > 0.3) rate = 0.3;    // 30%
    investmentReturnRate = rate;
}

int Bank::getReserve() const {
    return goldReserves;
}

int Bank::getNumLoans() const {
    return numLoans;
}

int Bank::getNumInvestments() const {
    return numInvestments;
}

int Bank::getGoldReserves() const {
    return goldReserves;
}

void Bank::setGoldReserves(int amount) {
    goldReserves = max(0, amount);
}

int Bank::getTotalLoans() const {
    return totalLoans;
}

int Bank::getLoanLimit() const {
    return loanLimit;
}

void Bank::setLoanLimit(int limit) {
    loanLimit = max(0, limit);
}

int Bank::getDefaultRisk() const {
    return defaultRisk;
}

void Bank::setDefaultRisk(int risk) {
    defaultRisk = max(0, min(100, risk));
}

int Bank::getFraudLevel() const {
    return fraudLevel;
}

void Bank::setFraudLevel(int level) {
    fraudLevel = max(0, min(100, level));
}

// Loan operations
bool Bank::takeLoan(int amount, int duration) {
    if (amount <= 0 || duration <= 0) {
        return false;
    }

    // Check if we're at the loan limit
    if (totalLoans + amount > loanLimit) {
        return false;
    }

    // Check if we have enough reserves
    if (amount > goldReserves) {
        return false;
    }

    // Check if we're at the maximum number of loans
    if (numLoans >= maxLoans) {
        // Create a new, larger array
        int newMaxLoans = maxLoans * 2;
        Loan** newLoans = new Loan * [newMaxLoans];

        // Copy existing loans to the new array
        for (int i = 0; i < numLoans; i++) {
            newLoans[i] = loans[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numLoans; i < newMaxLoans; i++) {
            newLoans[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] loans;
        loans = newLoans;
        maxLoans = newMaxLoans;
    }

    // Create a new loan
    string loanName = "Loan #" + to_string(numLoans + 1);
    Loan* newLoan = new Loan(loanName);
    newLoan->amount = amount;
    newLoan->interestRate = interestRate;
    newLoan->duration = duration;
    newLoan->turnsRemaining = duration;
    newLoan->isActive = true;

    // Add the loan
    loans[numLoans++] = newLoan;

    // Update bank state
    totalLoans += amount;
    goldReserves -= amount;

    return true;
}

// Repay a loan or part of a loan
void Bank::repayLoan(int amount) {
    if (amount <= 0 || numLoans == 0) {
        return;
    }

    // Find the oldest active loan
    Loan* oldestLoan = nullptr;
    int oldestIndex = -1;

    for (int i = 0; i < numLoans; i++) {
        if (loans[i] && loans[i]->isActive) {
            oldestLoan = loans[i];
            oldestIndex = i;
            break;
        }
    }

    if (!oldestLoan) {
        // No active loans
        return;
    }

    // Calculate interest
    int totalOwed = oldestLoan->amount;
    int turnsElapsed = oldestLoan->duration - oldestLoan->turnsRemaining;

    // Simple interest calculation
    int interest = (totalOwed * oldestLoan->interestRate * turnsElapsed) / 100;
    totalOwed += interest;

    // Apply payment
    int remaining = totalOwed - amount;

    if (remaining <= 0) {
        // Loan is fully repaid
        totalLoans -= oldestLoan->amount;
        goldReserves += amount - abs(remaining); // Return excess payment

        // Mark loan as inactive
        oldestLoan->isActive = false;
        oldestLoan->amount = 0;
        oldestLoan->turnsRemaining = 0;
    }
    else {
        // Partial repayment
        double repaymentRatio = (double)amount / totalOwed;
        int principalRepaid = (int)(oldestLoan->amount * repaymentRatio);

        oldestLoan->amount -= principalRepaid;
        totalLoans -= principalRepaid;
        goldReserves += amount;
    }
}

// Adjust interest rate based on economic conditions
void Bank::adjustInterestRate(int economicStability) {
    // Economic stability is 0-100
    // Higher stability = lower interest rates

    // Base adjustment - random factor
    double adjustment = randomDouble(-0.5, 0.5);

    // Economic stability effect (inverse relationship)
    double stabilityEffect = (50 - economicStability) / 50.0; // -1.0 to 1.0
    adjustment += stabilityEffect;

    // Default risk effect
    adjustment += defaultRisk / 100.0;

    // Apply adjustment
    setInterestRate(interestRate + adjustment);
}

// Handle potential fraud from corrupt leaders
void Bank::handleFraud(const Leader& leader) {
    int corruptionLevel = leader.getCorruption();

    if (corruptionLevel <= 0) {
        return; // No corruption, no fraud
    }

    // Calculate potential fraud amount based on corruption and reserves
    int potentialFraud = (goldReserves * corruptionLevel) / 1000; // 0.1% per corruption point

    // Chance of fraud occurring
    int fraudChance = corruptionLevel / 2; // 0-50%

    if (randomInt(1, 100) <= fraudChance) {
        int fraudAmount = randomInt(potentialFraud / 2, potentialFraud);

        // Subtract from reserves
        goldReserves -= fraudAmount;

        // Increase fraud level
        fraudLevel += randomInt(1, 5);
        if (fraudLevel > 100) fraudLevel = 100;
    }
}

// Conduct an audit to detect and reduce fraud
void Bank::audit() {
    // Reduce fraud level
    int reduction = randomInt(10, 30);
    fraudLevel = max(0, fraudLevel - reduction);

    // If there was significant fraud, there's a chance to recover some gold
    if (fraudLevel > 30) {
        int recoveryChance = fraudLevel / 2;

        if (randomInt(1, 100) <= recoveryChance) {
            int recoveredAmount = randomInt(100, 1000);
            goldReserves += recoveredAmount;

            // Further reduce fraud level
            fraudLevel = max(0, fraudLevel - 10);
        }
    }
}

// Process loan defaults
void Bank::processDefaults() {
    for (int i = 0; i < numLoans; i++) {
        if (loans[i] && loans[i]->isActive) {
            // Check for default based on default risk
            int defaultChance = defaultRisk / 10; // 0-10%

            if (randomInt(1, 100) <= defaultChance) {
                // Loan defaults - write off the loan
                totalLoans -= loans[i]->amount;
                loans[i]->isActive = false;

                // Increase default risk
                defaultRisk = min(100, defaultRisk + 5);
            }
        }
    }
}

// Update method required by Entity base class
void Bank::update() {
    if (!isActive) return;

    // Process loan payments and expirations
    for (int i = 0; i < numLoans; i++) {
        if (loans[i] && loans[i]->isActive) {
            // Decrement turns remaining
            loans[i]->turnsRemaining--;

            // Check if loan has expired
            if (loans[i]->turnsRemaining <= 0) {
                // Attempt to automatically collect payment
                int interestEarned = (loans[i]->amount * loans[i]->interestRate * loans[i]->duration) / 100;
                goldReserves += interestEarned;

                // Mark loan as inactive
                loans[i]->isActive = false;
            }
        }
    }

    // Process potential defaults
    processDefaults();

    // Random changes to default risk
    int riskChange = randomInt(-2, 3); // More likely to increase than decrease
    defaultRisk = max(1, min(100, defaultRisk + riskChange));

    // Gradually reduce fraud level if no active fraud
    if (fraudLevel > 0) {
        fraudLevel = max(0, fraudLevel - 1);
    }
}

// Save bank data to file
void Bank::save(ofstream& file) const {
    if (!file.is_open()) {
        throw runtime_error("Failed to save Bank: File not open");
    }

    // Call base class save first
    Entity::save(file);

    // Save Bank-specific attributes
    file << interestRate << endl;
    file << goldReserves << endl;
    file << totalLoans << endl;
    file << loanLimit << endl;
    file << defaultRisk << endl;
    file << fraudLevel << endl;

    // Save number of loans
    file << numLoans << endl;

    // Save each loan
    for (int i = 0; i < numLoans; i++) {
        if (loans[i]) {
            file << loans[i]->name << endl;
            file << loans[i]->amount << endl;
            file << loans[i]->interestRate << endl;
            file << loans[i]->duration << endl;
            file << loans[i]->turnsRemaining << endl;
            file << loans[i]->isActive << endl;
        }
    }
}

// Load bank data from file
void Bank::load(ifstream& file) {
    if (!file.is_open()) {
        throw runtime_error("Failed to load Bank: File not open");
    }

    // Call base class load first
    Entity::load(file);

    // Load Bank-specific attributes
    file >> interestRate;
    file >> goldReserves;
    file >> totalLoans;
    file >> loanLimit;
    file >> defaultRisk;
    file >> fraudLevel;

    // Clean up existing loans
    for (int i = 0; i < numLoans; i++) {
        delete loans[i];
        loans[i] = nullptr;
    }
    numLoans = 0;

    // Load number of loans
    int loadNumLoans;
    file >> loadNumLoans;
    file.ignore(); // Skip newline

    // Ensure we have enough space for loans
    if (loadNumLoans > maxLoans) {
        // Create a new, larger array
        Loan** newLoans = new Loan * [loadNumLoans];

        // Initialize all slots to nullptr
        for (int i = 0; i < loadNumLoans; i++) {
            newLoans[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] loans;
        loans = newLoans;
        maxLoans = loadNumLoans;
    }

    // Load each loan
    for (int i = 0; i < loadNumLoans; i++) {
        Loan* loan = new Loan("Temp");

        getline(file, loan->name);
        file >> loan->amount;
        file >> loan->interestRate;
        file >> loan->duration;
        file >> loan->turnsRemaining;
        file >> loan->isActive;
        file.ignore(); // Skip newline

        loans[numLoans++] = loan;
    }
}

// Provide a loan to the treasury
bool Bank::provideLoan(int amount, int duration, Treasury* treasury) {
    if (!treasury || amount <= 0 || duration <= 0) {
        return false;
    }

    // Check if we have enough reserves
    if (amount > goldReserves) {
        return false;
    }

    // Check if we're at the maximum number of loans
    if (numLoans >= maxLoans) {
        // Create a new, larger array
        int newMaxLoans = maxLoans * 2;
        Loan** newLoans = new Loan * [newMaxLoans];

        // Copy existing loans to the new array
        for (int i = 0; i < numLoans; i++) {
            newLoans[i] = loans[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numLoans; i < newMaxLoans; i++) {
            newLoans[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] loans;
        loans = newLoans;
        maxLoans = newMaxLoans;
    }

    // Create a new loan
    string loanName = "Treasury Loan #" + to_string(numLoans + 1);
    Loan* newLoan = new Loan(loanName);
    newLoan->amount = amount;
    newLoan->interestRate = interestRate;
    newLoan->duration = duration;
    newLoan->turnsRemaining = duration;
    newLoan->isActive = true;

    // Add the loan
    loans[numLoans++] = newLoan;

    // Update bank state
    goldReserves -= amount;

    // Add gold to treasury
    treasury->deposit(amount);

    return true;
}

// Repay loans from treasury
int Bank::repayLoans(int amount, Treasury* treasury) {
    if (!treasury || amount <= 0 || numLoans == 0) {
        return 0;
    }

    // Make sure treasury has enough gold
    if (treasury->getGold() < amount) {
        return 0;
    }

    int remainingAmount = amount;
    int totalRepaid = 0;

    // Find active loans and repay them in order
    for (int i = 0; i < numLoans && remainingAmount > 0; i++) {
        if (loans[i] && loans[i]->isActive) {
            // Calculate interest
            double interestRate = loans[i]->interestRate;
            int loanDuration = loans[i]->duration;
            int turnsElapsed = loanDuration - loans[i]->turnsRemaining;

            int loanAmount = loans[i]->amount;
            int interest = static_cast<int>(loanAmount * interestRate * turnsElapsed / 10.0);
            int totalOwed = loanAmount + interest;

            if (remainingAmount >= totalOwed) {
                // Can pay off the entire loan
                remainingAmount -= totalOwed;
                totalRepaid += totalOwed;

                // Mark loan as repaid
                loans[i]->isActive = false;
                loans[i]->amount = 0;
                goldReserves += loanAmount; // Return principal to reserves
            }
            else {
                // Partial repayment
                double ratio = static_cast<double>(remainingAmount) / totalOwed;
                int principalRepaid = static_cast<int>(loanAmount * ratio);

                loans[i]->amount -= principalRepaid;
                goldReserves += principalRepaid;

                totalRepaid += remainingAmount;
                remainingAmount = 0;
            }
        }
    }

    // Remove the gold from treasury
    if (totalRepaid > 0) {
        treasury->spend(totalRepaid);
    }

    return totalRepaid;
}

// Display all active loans and return total debt
int Bank::displayLoans() {
    int totalDebt = 0;
    bool hasActiveLoans = false;

    for (int i = 0; i < numLoans; i++) {
        if (loans[i] && loans[i]->isActive) {
            hasActiveLoans = true;

            double interestRate = loans[i]->interestRate;
            int loanDuration = loans[i]->duration;
            int turnsRemaining = loans[i]->turnsRemaining;
            int turnsElapsed = loanDuration - turnsRemaining;

            int loanAmount = loans[i]->amount;
            int interest = static_cast<int>(loanAmount * interestRate * turnsElapsed / 10.0);
            int totalOwed = loanAmount + interest;

            cout << (i + 1) << ". " << loans[i]->name << endl;
            cout << "   Principal: " << loanAmount << " gold" << endl;
            cout << "   Interest accrued: " << interest << " gold" << endl;
            cout << "   Total owed: " << totalOwed << " gold" << endl;
            cout << "   Time remaining: " << turnsRemaining << " / " << loanDuration << " turns" << endl;
            cout << endl;

            totalDebt += totalOwed;
        }
    }

    if (!hasActiveLoans) {
        cout << "No active loans." << endl;
    }

    return totalDebt;
}

// Display all active investments
void Bank::displayInvestments() {
    bool hasActiveInvestments = false;

    for (int i = 0; i < numInvestments; i++) {
        if (investments[i] && investments[i]->isActive) {
            hasActiveInvestments = true;

            double returnRate = investments[i]->returnRate;
            int investmentDuration = investments[i]->duration;
            int turnsRemaining = investments[i]->turnsRemaining;
            int turnsElapsed = investmentDuration - turnsRemaining;

            int investmentAmount = investments[i]->amount;
            int returns = static_cast<int>(investmentAmount * returnRate * investmentDuration / 10.0);
            int totalReturn = investmentAmount + returns;

            cout << (i + 1) << ". " << investments[i]->name << endl;
            cout << "   Principal: " << investmentAmount << " gold" << endl;
            cout << "   Expected returns: " << returns << " gold" << endl;
            cout << "   Total value at maturity: " << totalReturn << " gold" << endl;
            cout << "   Time remaining: " << turnsRemaining << " / " << investmentDuration << " turns" << endl;
            cout << endl;
        }
    }

    if (!hasActiveInvestments) {
        cout << "No active investments." << endl;
    }
}

// Make an investment from the treasury
bool Bank::makeInvestment(int amount, int duration, Treasury* treasury) {
    if (!treasury || amount <= 0 || duration <= 0) {
        return false;
    }

    // Check if treasury has enough gold
    if (treasury->getGold() < amount) {
        return false;
    }

    // Check if we're at the maximum number of investments
    if (numInvestments >= maxInvestments) {
        // Create a new, larger array
        int newMaxInvestments = maxInvestments * 2;
        Investment** newInvestments = new Investment * [newMaxInvestments];

        // Copy existing investments to the new array
        for (int i = 0; i < numInvestments; i++) {
            newInvestments[i] = investments[i];
        }

        // Initialize remaining slots to nullptr
        for (int i = numInvestments; i < newMaxInvestments; i++) {
            newInvestments[i] = nullptr;
        }

        // Delete the old array and update pointers
        delete[] investments;
        investments = newInvestments;
        maxInvestments = newMaxInvestments;
    }

    // Create new investment
    string investmentName = "Investment #" + to_string(numInvestments + 1);
    Investment* newInvestment = new Investment(investmentName);
    newInvestment->amount = amount;
    newInvestment->returnRate = investmentReturnRate;
    newInvestment->duration = duration;
    newInvestment->turnsRemaining = duration;
    newInvestment->isActive = true;

    // Add the investment
    investments[numInvestments++] = newInvestment;

    // Remove gold from treasury
    treasury->spend(amount);

    // Add gold to bank reserves
    goldReserves += amount;

    return true;
}