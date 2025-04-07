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

    void readData(const string &path)
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

    static double func(double x, double y)
    {
        return 2 * x;
    }

    double rkstep(double x, double y0, double h)
    {
        k1 = h * func(x, y0);
        k2 = h * func(x + h / 2, y0 + k1 / 2);
        return y0 + k2;
    }

    double diff(double x, double y0)
    {
        double step = h * sign;
        double y_h1 = rkstep(x, y0, step);

        double y_h21 = rkstep(x, y0, step / 2);
        double y_h22 = rkstep(x + step / 2, y_h21, step / 2);
        return fabs(y_h1 - y_h22) / 3.0;
    }

    void print(double x)
    {
        cout << "X: " << x << " Y: " << y << endl;
    }

    void printStep(double x)
    {
        cout << "h: " << sign * h << ";  X: " << x << "  Y: " << y << "  E: " << diff(x, y) << endl;
    }

    void printPointIntfo()
    {
        cout << "\nЧисло точек интегрирования: " << dotsCount << "\nТочность не достигается в "
             << badDotsCount << " точк.\nМинимальное число шагов интегрирования: " << minDotsCount << endl;
    }
};


int Calculate(System &system)
{
    double x, bound, y_new, diff;
    if (system.C == system.A)
    {
        system.sign = 1;
        x = system.A;
        bound = system.B;
    }
    else if (system.C == system.B)
    {
        system.sign = -1;
        x = system.B;
        bound = system.A;
    }
    else
    {
        cout << "Значение C не совпадает ни с одним значением границ участка.\n";
        return 2;
    }

    system.print(x);
    while (fabs(bound - (x + system.h)) >= system.hMin && (system.sign == 1 ? bound > (x + system.h) : bound < (x - system.h)))
    {
        y_new = system.rkstep(x, system.y, system.h * system.sign);
        diff = system.diff(x, system.y);

        if (diff > system.eps)
        {
            if (system.h == system.hMin)
            {
                system.y = y_new;
                x += system.h * system.sign;
                system.printStep(x);
                system.dotsCount++;
                system.badDotsCount++;
                system.minDotsCount++;
            }
            else
            {
                system.h = max(system.h / 2, system.hMin);
            }
        }
        else
        {
            system.y = y_new;
            x += system.h * system.sign;
            system.printStep(x);
            system.dotsCount++;
            if (system.h == system.hMin)
                system.minDotsCount++;
                // 2**S, s - порядок метода
            if (diff < (system.eps / 4.0))
                system.h = min(fabs(system.h * 2), fabs(bound - x));
        }
    }

    // cout << "end\n";

    if (fabs(bound - x) >= 2 * system.hMin)
    {
        system.minDotsCount += 2;
        system.dotsCount += 2;

        // 1-st
        system.h = fabs(bound - x) - system.hMin;
        y_new = system.rkstep(x, system.y, system.h * system.sign);
        diff = system.diff(x, system.y);

        if (diff > system.eps)
            system.badDotsCount++;

        system.y = y_new;
        x += system.sign * system.h;
        system.printStep(x);

        // 2-nd
        system.h = system.hMin;
        y_new = system.rkstep(x, system.y, system.h * system.sign);
        diff = system.diff(x, system.y);
        system.y = y_new;
        x = bound;
        system.printStep(x);
        if (diff > system.eps)
            system.badDotsCount++;

        system.printPointIntfo();

        if (system.badDotsCount != 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (fabs(bound - x) <= 1.5 * system.hMin)
    {
        system.h = fabs(bound - x);
        y_new = system.rkstep(x, system.y, system.h * system.sign);
        diff = system.diff(x, system.y);

        system.dotsCount++;
        system.minDotsCount++;
        if (diff > system.eps)
            system.badDotsCount++;

        x += system.sign * system.h;
        system.y = y_new;
        system.printStep(x);
        system.printPointIntfo();

        if (system.badDotsCount != 0)
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
        system.minDotsCount += 2;
        system.dotsCount += 2;

        // 1-st
        system.h = fabs(bound - x) / 2;
        y_new = system.rkstep(x, system.y, system.h * system.sign);
        diff = system.diff(x, system.y);

        if (diff > system.eps)
            system.badDotsCount++;

        system.y = y_new;
        x += system.h;
        system.printStep(x);

        // 2-nd
        y_new = system.rkstep(x, system.y, system.h * system.sign);
        diff = system.diff(x, system.y);
        system.y = y_new;
        x = bound;
        system.printStep(x);
        if (diff > system.eps)
            system.badDotsCount++;

        system.printPointIntfo();

        if (system.badDotsCount != 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
