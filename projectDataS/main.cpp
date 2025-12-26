#include <iostream>
#include "Restaurant.h"
using namespace std;

int main() {
    cout << endl;
    cout << "========================================" << endl;
    cout << "   Restaurant Management System         " << endl;
    cout << "   Phase 2 - Complete Simulation        " << endl;
    cout << "========================================" << endl;
    cout << endl;

    Restaurant restaurant;

    string filename;
    cout << "Enter input file name: ";
    getline(cin, filename);

    if (!restaurant.loadInputFile(filename)) {
        cout << "Failed to load file." << endl;
        return 1;
    }

    restaurant.runPhase2Simulation();

    return 0;
}

