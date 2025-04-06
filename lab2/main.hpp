#include <iostream>

#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

class System
{
public:
    double A, B, C, yC;
    double h, hMin, eps;
    double y;
    double k1, k2;
    int dotsCount = 0, badDotsCount = 0, minDotsCount = 1, sign;

    void InputFromFile(const string &path)
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

    double rungeKuttaStep(double x, double y0, double h)
    {
        k1 = h * func_calc(x, y0);
        k2 = h * func_calc(x + h / 2, y0 + k1 / 2);
        return y0 + k2;
    }

    double FindDiff(double x, double y0)
    {
        double step = h * sign;
        double y_h1 = rungeKuttaStep(x, y0, step);

        double y_h21 = rungeKuttaStep(x, y0, step / 2);
        double y_h22 = rungeKuttaStep(x + step / 2, y_h21, step / 2);
        return fabs(y_h1 - y_h22) / 3.0;
    }

    void PrintInfo(double x)
    {
        cout << "X: " << x << " Y: " << y << endl;
    }

    void PrintStep(double x)
    {
        cout << "h: " << sign * h << ";  X: " << x << "  Y: " << y << "  E: " << FindDiff(x, y) << endl;
    }

    void PrintPointsStats()
    {
        cout << "\nЧисло точек интегрирования: " << dotsCount << "\nТочность не достигается в "
             << badDotsCount << " точк.\nМинимальное число шагов интегрирования: " << minDotsCount << endl;
    }
};


int Calculate(System &koshi)
{
    double x, bound, y_new, diff;
    if (koshi.C == koshi.A)
    {
        koshi.sign = 1;
        x = koshi.A;
        bound = koshi.B;
    }
    else if (koshi.C == koshi.B)
    {
        koshi.sign = -1;
        x = koshi.B;
        bound = koshi.A;
    }
    else
    {
        cout << "Значение C не совпадает ни с одним значением границ участка.\n";
        return 2;
    }

    koshi.PrintInfo(x);
    while (fabs(bound - (x + koshi.h)) >= koshi.hMin && (koshi.sign == 1 ? bound > (x + koshi.h) : bound < (x - koshi.h)))
    {
        y_new = koshi.rungeKuttaStep(x, koshi.y, koshi.h * koshi.sign);
        diff = koshi.FindDiff(x, koshi.y);

        if (diff > koshi.eps)
        {
            if (koshi.h == koshi.hMin)
            {
                koshi.y = y_new;
                x += koshi.h * koshi.sign;
                koshi.PrintStep(x);
                koshi.dotsCount++;
                koshi.badDotsCount++;
                koshi.minDotsCount++;
            }
            else
            {
                koshi.h = max(koshi.h / 2, koshi.hMin);
            }
        }
        else
        {
            koshi.y = y_new;
            x += koshi.h * koshi.sign;
            koshi.PrintStep(x);
            koshi.dotsCount++;
            if (koshi.h == koshi.hMin)
                koshi.minDotsCount++;
            if (diff < (koshi.eps / 4.0))
                koshi.h = min(fabs(koshi.h * 2), fabs(bound - x));
        }
    }

    // cout << "end\n";

    if (fabs(bound - x) >= 2 * koshi.hMin)
    {
        koshi.minDotsCount += 2;
        koshi.dotsCount += 2;

        // 1-st
        koshi.h = fabs(bound - x) - koshi.hMin;
        y_new = koshi.rungeKuttaStep(x, koshi.y, koshi.h * koshi.sign);
        diff = koshi.FindDiff(x, koshi.y);

        if (diff > koshi.eps)
            koshi.badDotsCount++;

        koshi.y = y_new;
        x += koshi.sign * koshi.h;
        koshi.PrintStep(x);

        // 2-nd
        koshi.h = koshi.hMin;
        y_new = koshi.rungeKuttaStep(x, koshi.y, koshi.h * koshi.sign);
        diff = koshi.FindDiff(x, koshi.y);
        koshi.y = y_new;
        x = bound;
        koshi.PrintStep(x);
        if (diff > koshi.eps)
            koshi.badDotsCount++;

        koshi.PrintPointsStats();

        if (koshi.badDotsCount != 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (fabs(bound - x) <= 1.5 * koshi.hMin)
    {
        koshi.h = fabs(bound - x);
        y_new = koshi.rungeKuttaStep(x, koshi.y, koshi.h * koshi.sign);
        diff = koshi.FindDiff(x, koshi.y);

        koshi.dotsCount++;
        koshi.minDotsCount++;
        if (diff > koshi.eps)
            koshi.badDotsCount++;

        x += koshi.sign * koshi.h;
        koshi.y = y_new;
        koshi.PrintStep(x);
        koshi.PrintPointsStats();

        if (koshi.badDotsCount != 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        koshi.minDotsCount += 2;
        koshi.dotsCount += 2;

        // 1-st
        koshi.h = fabs(bound - x) / 2;
        y_new = koshi.rungeKuttaStep(x, koshi.y, koshi.h * koshi.sign);
        diff = koshi.FindDiff(x, koshi.y);

        if (diff > koshi.eps)
            koshi.badDotsCount++;

        koshi.y = y_new;
        x += koshi.h;
        koshi.PrintStep(x);

        // 2-nd
        y_new = koshi.rungeKuttaStep(x, koshi.y, koshi.h * koshi.sign);
        diff = koshi.FindDiff(x, koshi.y);
        koshi.y = y_new;
        x = bound;
        koshi.PrintStep(x);
        if (diff > koshi.eps)
            koshi.badDotsCount++;

        koshi.PrintPointsStats();

        if (koshi.badDotsCount != 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
