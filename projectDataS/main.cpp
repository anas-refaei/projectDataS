#include <iostream>
#include "Restaurant.h"
//#include "CMUgraphicsLib\CMUgraphics.h"
#include "GUI/GUI.h"
using namespace std;

int main() {
    // Create GUI and get mode selection
    GUI* pGUI = new GUI;
    PROG_MODE mode = pGUI->getGUIMode();

    // Create restaurant and set GUI
    Restaurant restaurant;
    restaurant.setGUI(pGUI);
    restaurant.setMode(mode);

    // Get input filename
    pGUI->PrintMessage("Enter input filename: ");
    string filename = pGUI->GetString();

    // Load input file
    if (!restaurant.loadInputFile(filename)) {
        pGUI->PrintMessage("Failed to load file! Click to exit.");
        pGUI->waitForClick();
        delete pGUI;
        return 1;
    }

    // Run simulation with selected GUI mode
    restaurant.runSimulationWithGUI();

    // Cleanup
    delete pGUI;
    return 0;
}
