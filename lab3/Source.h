#include "System.h"

void CoutAllInformationAboutSystem(System system)
{
    for (int i = 0; i < system.pointsCount; i++)
    {
        cout << i + 1 << ") X: " << system.X[i] << "; Y: " << system.Y[i] << "; Y': " << system.YY[i];
        cout << "; x^3: " << system.X[i] * system.X[i] * system.X[i] << "; 3x^2: " << 3 * system.X[i] * system.X[i];
        cout << endl;
    }
    cout << "\n\n";
}