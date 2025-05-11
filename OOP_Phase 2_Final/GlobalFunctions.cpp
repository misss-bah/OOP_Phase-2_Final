#include "Stronghold.h"
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

// Random number generator for integers in range [min, max]
int randomInt(int min, int max) {
    // Ensure max is greater than min
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }

    // Generate random number
    return min + (rand() % (max - min + 1));
}

// Random number generator for doubles in range [min, max]
double randomDouble(double min, double max) {
    // Ensure max is greater than min
    if (min > max) {
        double temp = min;
        min = max;
        max = temp;
    }

    // Generate random double
    double factor = (double)rand() / RAND_MAX;
    return min + factor * (max - min);
}

// Format current date and time as a string
string currentDateTime() {
    time_t now = time(0);

    // Convert time_t to string using string streams
    string timestamp = "Timestamp: ";
    timestamp += to_string(now);

    return timestamp;
}

// Trim leading and trailing whitespace from a string
void trimString(string& str) {
    // Trim leading whitespace
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start != string::npos) {
        str = str.substr(start);
    }
    else {
        // String contains only whitespace
        str = "";
        return;
    }

    // Trim trailing whitespace
    size_t end = str.find_last_not_of(" \t\n\r");
    if (end != string::npos) {
        str = str.substr(0, end + 1);
    }
}