#include "main.hpp"

int main()
{
    setlocale(LC_ALL, "rus");
    while (true)
    {
        cout << "1) Ввести данные с файла\n2) Выход" << endl;
        int choice;
        cin >> choice;
        if (choice == 2)
            break;
        if (choice == 1)
        {
            cout << "\nВведите название входного файла: ";
            string inputPath;
            cin >> inputPath;

            System system;
            system.readData(inputPath + ".txt");
            int result = Calculate(system);
            cout << "IER: " << result << endl
                 << endl;
        }
        else
        {
            cout << "Неправильный ввод.\n\n";
            return 0;
        }
    }
    return 0;
}
