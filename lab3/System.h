#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

enum class Direction { Forward, Backward };

class System
{
public:
    double alpha1, beta1, gamma1;
    double alpha2, beta2, gamma2;
    double a, b;
    int pointsCount;
    vector<double> X;
    vector<double> U;
    vector<double> V;
    vector<double> W;
    vector<double> alpha;
    vector<double> beta;
    vector<double> gamma;
    vector<double> Y;
    vector<double> YY;

    static double Px(double x)
    {
        return -1;
        // return 0;
    }

    static double Qx(double x)
    {
        return 0;
    }

    static double Fx(double x)
    {
        return 0;
        // return 6 * x;

    }

    static double UderivRight(double x, double u, double v)
    {
        return Px(x) * u + v;
    }

    static double VderivRight(double x, double u)
    {
        return Qx(x) * u;
    }

    static double WderivRight(double x, double u)
    {
        return Fx(x) * u;
    }

    System(const string& path)
    {
        ifstream file(path);
        if (!file)
        {
            cerr << "Ошибка при поиске файла " << path << endl;
            return;
        }

        file >> alpha1 >> beta1 >> gamma1 >> alpha2 >> beta2 >> gamma2;

        file >> a >> b >> pointsCount;

        X.resize(pointsCount);
        U.resize(pointsCount);
        V.resize(pointsCount);
        W.resize(pointsCount);
        alpha.resize(pointsCount);
        beta.resize(pointsCount);
        gamma.resize(pointsCount);
        Y.resize(pointsCount);
        YY.resize(pointsCount);

        for (int i = 0; i < pointsCount; i++)
        {
            int index;
            double value;
            file >> index >> value;
            X[i] = value;
        }

        file.close();
    }

    // Решает систему (u', v', w') на одном шаге
    void RungeKuttaSystemStep(double x, double& u, double& v, double& w, double h)
    {
        // Сохраняем начальные значения
        double u0 = u, v0 = v, w0 = w;

        // k1
        double ku1 = h * UderivRight(x, u0, v0);
        double kv1 = h * VderivRight(x, u0);
        double kw1 = h * WderivRight(x, u0);

        // k2
        double ku2 = h * UderivRight(x + h / 2.0, u0 + ku1 / 2.0, v0 + kv1 / 2.0);
        double kv2 = h * VderivRight(x + h / 2.0, u0 + ku1 / 2.0);
        double kw2 = h * WderivRight(x + h / 2.0, u0 + ku1 / 2.0);

        // k3
        double ku3 = h * UderivRight(x + h / 2.0, u0 + (ku1 + ku2) / 4.0, v0 + (kv1 + kv2) / 4.0);
        double kv3 = h * VderivRight(x + h / 2.0, u0 + (ku1 + ku2) / 4.0);
        double kw3 = h * WderivRight(x + h / 2.0, u0 + (ku1 + ku2) / 4.0);

        // k4
        double ku4 = h * UderivRight(x + h, u0 - ku2 + 2.0 * ku3, v0 - kv2 + 2.0 * kv3);
        double kv4 = h * VderivRight(x + h, u0 - ku2 + 2.0 * ku3);
        double kw4 = h * WderivRight(x + h, u0 - ku2 + 2.0 * ku3);

        // k5
        double ku5 = h * UderivRight(x + 2.0 * h / 3.0, u0 + 7.0 * ku1 / 27.0 + 10.0 * ku2 / 27.0 + ku4 / 27.0, v0 + 7.0 * kv1 / 27.0 + 10.0 * kv2 / 27.0 + kv4 / 27.0);
        double kv5 = h * VderivRight(x + 2.0 * h / 3.0, u0 + 7.0 * ku1 / 27.0 + 10.0 * ku2 / 27.0 + ku4 / 27.0);
        double kw5 = h * WderivRight(x + 2.0 * h / 3.0, u0 + 7.0 * ku1 / 27.0 + 10.0 * ku2 / 27.0 + ku4 / 27.0);

        // k6
        double ku6 = h * UderivRight(x + h / 5.0, u0 + (28.0 * ku1 - 125.0 * ku2 + 546.0 * ku3 + 54.0 * ku4 - 378.0 * ku5) / 625.0, v0 + (28.0 * kv1 - 125.0 * kv2 + 546.0 * kv3 + 54.0 * kv4 - 378.0 * kv5) / 625.0);
        double kv6 = h * VderivRight(x + h / 5.0, u0 + (28.0 * ku1 - 125.0 * ku2 + 546.0 * ku3 + 54.0 * ku4 - 378.0 * ku5) / 625.0);
        double kw6 = h * WderivRight(x + h / 5.0, u0 + (28.0 * ku1 - 125.0 * ku2 + 546.0 * ku3 + 54.0 * ku4 - 378.0 * ku5) / 625.0);

        // Обновляем переменные
        u += (14.0 * ku1 + 35.0 * ku4 + 162.0 * ku5 + 125.0 * ku6) / 336.0;
        v += (14.0 * kv1 + 35.0 * kv4 + 162.0 * kv5 + 125.0 * kv6) / 336.0;
        w += (14.0 * kw1 + 35.0 * kw4 + 162.0 * kw5 + 125.0 * kw6) / 336.0;
    }

    void CalculateOneWay(Direction dir)
    {
        auto& out_u = (dir == Direction::Forward) ? U : alpha;
        auto& out_v = (dir == Direction::Forward) ? V : beta;
        auto& out_w = (dir == Direction::Forward) ? W : gamma;

        double x = (dir == Direction::Forward) ? a : b;
        double u = (dir == Direction::Forward) ? alpha1 : alpha2;
        double v = (dir == Direction::Forward) ? -beta1 : -beta2;
        double w = (dir == Direction::Forward) ? gamma1 : gamma2;

        int start = (dir == Direction::Forward) ? 0 : pointsCount - 1;
        int end = (dir == Direction::Forward) ? pointsCount : -1;
        int step = (dir == Direction::Forward) ? 1 : -1;

        double h;
        for (int i = start; i != end; i += step)
        {
            if (i != start)
            {
                h = (dir == Direction::Forward)
                    ? (X[i] - X[i - 1])
                    : (X[i] - X[i + 1]);

                RungeKuttaSystemStep(x, u, v, w, h);
                x += h;
            }

            out_u[i] = u;
            out_v[i] = v;
            out_w[i] = w;
        }

    }

    void CalculateSystem()
    {
        for (int i = 0; i < pointsCount; i++)
        {
            long double a11 = U[i];
            long double a12 = -V[i];
            long double b1 = W[i];

            long double a21 = alpha[i];
            long double a22 = -beta[i];
            long double b2 = gamma[i];

            long double det = a11 * a22 - a12 * a21;        

            // Крамер для 2x2
            YY[i] = (b1 * a22 - b2 * a12) / det;
            Y[i] = (a11 * b2 - a21 * b1) / det;
        }
    }
};
