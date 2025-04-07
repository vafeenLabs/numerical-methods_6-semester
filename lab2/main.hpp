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
        return 2 * x + y - x * x;
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
        double y1 = rkstep(x, y, step);
        double y0_5 = rkstep(x, y, step / 2);
        double y0_5_1 = rkstep(x + step / 2, y0_5, step / 2);
        return fabs(y0_5_1 - y1) / 3.0;
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
        // x += system.sign * system.h;
        x = bound - system.h;
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
    else if (fabs(bound - x) <= 1.5 * system.hMin && fabs(bound - x) > 10e-5)
    {
        system.h = fabs(bound - x);
        y_new = system.rkstep(x, system.y, system.h * system.sign);
        diff = system.diff(x, system.y);

        system.dotsCount++;
        system.minDotsCount++;
        if (diff > system.eps)
            system.badDotsCount++;
        cout << "x:" << x << ",bound:" << bound << ",fabs(bound - x):" << fabs(bound - x) << '\n';
        x = bound;
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
    else if (fabs(bound - x) > 10e-5)
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
        {
            system.badDotsCount++;
        }
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
    system.printPointIntfo();
    return 0;
};
