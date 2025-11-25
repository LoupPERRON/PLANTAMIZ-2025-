#pragma once
#ifndef MENU_H
#define MENU_H

#include <iostream>

// Placeholders for the functions that exist in your menu implementation (menu.cpp).
// Adjust these prototypes to match the actual function names and signatures
// that you already have in the project.
void displayMainMenu();
int getMenuChoice();
void addPlant();
void listPlants();
void removePlant();
void savePlants();
void loadPlants();

// Wrapper that calls the functions from the real menu implementation.
// Include this header where you want to run the menu (e.g., main.cpp).
inline void RunMenu()
{
    bool running = true;
    while (running)
    {
        displayMainMenu();
        int choice = getMenuChoice();

        switch (choice)
        {
            case 1: addPlant();   break;
            case 2: listPlants(); break;
            case 3: removePlant();break;
            case 4: savePlants(); break;
            case 5: loadPlants(); break;
            case 0: running = false; break;
            default: std::cout << "Invalid option, please try again.\n"; break;
        }
    }
}

#endif // MENU_H