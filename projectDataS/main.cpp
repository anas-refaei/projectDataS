#include <iostream>
#include "Restaurant.h"
using namespace std;

int main() {
    cout << "=====================================" << endl;
    cout << " Restaurant Management System" << endl;
    cout << " Phase 1 - Data Structures Demo" << endl;
    cout << "=====================================" << endl;

    Restaurant restaurant;

    string filename;
    cout << "\nEnter input file name: ";
    getline(cin, filename);

    if (!restaurant.loadInputFile(filename)) {
        cout << "Failed to load file." << endl;
        return 1;
    }

    restaurant.runPhase1Simulation();

    return 0;
}
