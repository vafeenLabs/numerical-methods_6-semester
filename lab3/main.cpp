#include "System.h"
#include"Source.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");
    // while (true)
    // {
        // cout << "1) Ввести данные с файла\n2) Выход\n\n";
        // int choice;
        // cin >> choice;

        // if (choice == 1)
        // {
            cout << "\nВведите название входного файла: ";
            string path;
            cin >> path;

            System system(path + ".txt");
            system.CalculateOneWay(Direction::Forward);
            system.CalculateOneWay(Direction::Backward);

            system.CalculateSystem();
            CoutAllInformationAboutSystem(system);
            
        // }
        // else if (choice == 2)
        // {
        //     // break;
        // }
        // else
        // {
        //     cout << "Invalid input\n";
        //     return 0;
        // }
    // }
    return 0;
}