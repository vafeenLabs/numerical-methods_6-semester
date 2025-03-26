#include "Koshi.h"

int Calculate(Koshi& koshi)
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
    while (fabs(bound - (x + koshi.h)) >= koshi.hMin)
    {
        y_new = koshi.rungeKuttaStep(x, koshi.h);
        diff = koshi.FindDiff(x);

        if (diff > koshi.eps)
        {
            if (koshi.h == koshi.hMin)
            {
                koshi.y = y_new;
                x += koshi.h * koshi.sign;
                koshi.PrintStep(x);
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
                koshi.h *= 2;
        }
    }

    if (fabs(bound - x) >= 2 * koshi.hMin)
    {
        koshi.minDotsCount += 2;
        koshi.dotsCount += 2;

        // 1-st
        koshi.h = fabs(bound - x) - koshi.hMin;
        y_new = koshi.rungeKuttaStep(x, koshi.h);
        diff = koshi.FindDiff(x);

        if (diff > koshi.eps)
            koshi.badDotsCount++;

        koshi.y = y_new;
        x += koshi.h;
        koshi.PrintStep(x);

        // 2-nd
        koshi.h = koshi.hMin;
        y_new = koshi.rungeKuttaStep(x, koshi.h);
        diff = koshi.FindDiff(x);
        koshi.y = y_new;
        x = bound;
        koshi.PrintStep(x);
        if (diff > koshi.eps)
            koshi.badDotsCount++;

        koshi.PrintPointsStats();

        if (koshi.badDotsCount != 0)
            return 1;
        else
            return 0;
    }
    else if (fabs(bound - x) <= 1.5 * koshi.hMin)
    {
        koshi.h = fabs(bound - x);
        y_new = koshi.rungeKuttaStep(x, koshi.h);
        diff = koshi.FindDiff(x);

        koshi.dotsCount++;
        koshi.minDotsCount++;
        if (diff > koshi.eps)
            koshi.badDotsCount++;

        x += koshi.h;
        koshi.y = y_new;
        koshi.PrintStep(x);
        koshi.PrintPointsStats();

        if (koshi.badDotsCount != 0)
            return 1;
        else
            return 0;
    }
    else
    {
        koshi.minDotsCount += 2;
        koshi.dotsCount += 2;

        // 1-st
        koshi.h = fabs(bound - x) / 2;
        y_new = koshi.rungeKuttaStep(x, koshi.h);
        diff = koshi.FindDiff(x);

        if (diff > koshi.eps)
            koshi.badDotsCount++;

        koshi.y = y_new;
        x += koshi.h;
        koshi.PrintStep(x);

        // 2-nd
        y_new = koshi.rungeKuttaStep(x, koshi.h);
        diff = koshi.FindDiff(x);
        koshi.y = y_new;
        x = bound;
        koshi.PrintStep(x);
        if (diff > koshi.eps)
            koshi.badDotsCount++;

        koshi.PrintPointsStats();

        if (koshi.badDotsCount != 0)
            return 1;
        else
            return 0;
    }
}

