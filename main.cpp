#include <iostream>
#include "game.h"
using namespace std;

// Main function
// Entry point of the game
int main() {
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "|\t\t\t\tWELCOME OF LEGEND OF VALOR GAME                            \t |" << endl;
    cout << "--------------------------------------------------------------------------------------------------\n" << endl;
    cout << "--------------------------------------------          --------------------------------------------" << endl;
    cout << "|          1- NEW GAME                     |    " << "      |              2- LOAD GAME                |" << endl;
    cout << "--------------------------------------------          --------------------------------------------\n" << endl;
    cout << "ENTER ACTION:      ";
    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 2) {
        cout << "INVALID CHOICE! SELECT ONLY (1 or 2): ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    Game game;
    if (choice == 1) {
        game.createPlayer();
    }
    else {
        game.loadGame();
        if (!game.player) {
            cout << "FAILED TO LOAD GAME. STARTING NEW GAME...\n";
            game.createPlayer();
        }
    }
    if (game.player) { // Ensure player exists before running
        game.run();
    }
    else {
        cout << "ERROR NO PLAYER CREATING. EXITTING THE GAME\n";
    }
    return 0;
}
