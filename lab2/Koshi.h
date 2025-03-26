#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

class Koshi {
public:
    double A, B, C, yC;
    double h, hMin, eps;
    double y;
    double k1, k2;
    int dotsCount = 0, badDotsCount = 0, minDotsCount = 1, sign;

    Koshi() { }

    void InputFromFile(const string& path)
    {
        ifstream inputFile(path);
        if (!inputFile)
        {
            cerr << "Ошибка открытия файла входных данных!" << endl;
            exit(2);
        }
        inputFile >> A >> B >> C >> yC;
        inputFile >> hMin >> eps;
        inputFile.close();
        y = yC;
        h = fabs(B - A) / 10;
    }

    static double func_calc(double x, double y)
    {
        return 2 * x;
    }

    double rungeKuttaStep(double x, double step)
    {
        step *= sign;
        k1 = step * func_calc(x, y);
        k2 = step * func_calc(x + step / 2, y + k1 / 2);
        return y + k2;
    }

    double FindDiff(double x)
    {
        double y_h1 = rungeKuttaStep(x, h);

        double y_h2 = rungeKuttaStep(x, h / 2);
        y_h2 = rungeKuttaStep(x + h / 2, h / 2);
        return fabs(y_h1 - y_h2) / 3.0;
    }

    void PrintInfo(double x)
    {
        cout << "X: " << x << " Y: " << y << endl;
    }

    void PrintStep(double x)
    {
        cout << "h: " << h << ";  X: " << x << "  Y: " << y << "  E: " << FindDiff(x) << endl;
    }

    void PrintPointsStats() {
        cout << "\nЧисло точек интегрирования: " << dotsCount << "; Точность не достигается в "
            << badDotsCount << " точк.; Минимальное число шагов интегрирования: " << minDotsCount << endl;
    }
};