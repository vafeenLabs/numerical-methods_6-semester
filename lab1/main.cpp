#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;

const double EPS = 1e-6;

class Table {
private:
    vector<double> X;
    vector<double> Y;
    vector<double> coeff;
    vector<double> YY;
    int N;
    int IER;

    // Проверка возрастания X
    bool checkAscending() {
        double diff = X[1] - X[0];
        for (int i = 1; i < N; ++i) {
            if (X[i] <= X[i - 1] || (X[i] - X[i - 1]) != diff) return false;
        }
        return true;
    }

    // Вычисление третьих разностей
    vector<double> calcThirdDifferences() {
        vector<double> d3;

        for (int i = 0; i <= N - 4; ++i) {
            double diff = Y[i + 3] - 3 * Y[i + 2] + 3 * Y[i + 1] - Y[i];
            d3.push_back(diff);
        }
        
        return d3;
    }

    vector<double> gaussSolve(vector<vector<double>> a, vector<double> y) {
        const int n = 4;
        vector<double> res(n);

        // Прямой ход
        for (int row = 0; row < n; ++row) {
            // Нормировка строки
            double div = a[row][row];
            for (int col = 0; col < n; ++col) a[row][col] /= div;
            y[row] /= div;

            // Исключение переменной
            for (int i = row + 1; i < n; ++i) {
                double factor = a[i][row];
                for (int col = 0; col < n; ++col)
                    a[i][col] -= a[row][col] * factor;
                y[i] -= y[row] * factor;
            }
        }

        // Обратный ход
        for (int row = n - 1; row >= 0; --row) {
            res[row] = y[row];
            for (int i = row + 1; i < n; ++i) {
                res[row] -= a[row][i] * res[i];
            }
            // не помню зачем тут закомменчено было, но лучше попробовать и так и так 
            // if (fmod(res[row], 1.0) < EPS)
            // {
            //     cout << fmod(res[row], 1.0) << " ";
            //     res[row] = trunc(res[row]);
                
            // }
            
        }
        return res;
    }

    // Вычисление коэффициентов многочлена
    void calculateCoeff() {
        vector<vector<double>> mat(4, vector<double>(4));
        vector<double> y(4);

        // Используем первые 4 точки
        for (int i = 0; i < 4; ++i) {
            double x = X[i];
            mat[i][0] = 1;
            mat[i][1] = x;
            mat[i][2] = x * x;
            mat[i][3] = x * x * x;
            y[i] = YY[i];
        }

        coeff = gaussSolve(mat, y);
    }

public:
    Table() : IER(-1), N(0) {}

    // Загрузка данных из файла
    bool loadFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin) return false;

        fin >> N;
        X.resize(N);
        Y.resize(N);
        YY.resize(N);
        coeff.resize(4);

        for (int i = 0; i < N; ++i) fin >> X[i];
        for (int i = 0; i < N; ++i) fin >> Y[i];

        return true;
    }

    // Геттеры
    int getIER() const { return IER; }
    const vector<double>& getX() const { return X; }
    const vector<double>& getYY() const { return YY; }

    // Поиск и исправление ошибки
    void findAndFixError() {
        IER = -1;
        YY = Y;

        if (!checkAscending()) {
            IER = 3;
            return;
        }

        if (N < 4) {
            IER = 2;
            return;
        }

        auto d3 = calcThirdDifferences();
        if (d3.empty()) {
            IER = 2;
            return;
        }

        // Находим наиболее частое значение третьей разности
        map<double, int> counts; //[val, cnt]
        for (auto val : d3) counts[val]++;

        int max_count = 0;
        for (auto& entry : counts) {
            if (entry.second > max_count) max_count = entry.second;
        }

        vector<double> candidate_consts;
        for (auto& entry : counts) {
            if (entry.second == max_count)
            {
                candidate_consts.push_back(entry.first);
            }
        }
        
        bool error_fixed = false;
        double final_const = 0;
        int error_index = -1;
        double corrected_value = 0;

        if (candidate_consts.size() != 1)
        {
            for (auto const_d3 : candidate_consts)
            {

                // Поиск позиции ошибки
                for (int i = 0; i < d3.size(); ++i) {
                    if (fabs(d3[i] - const_d3) > EPS) {
                        for (int j = 0; j < 4; ++j) {
                            int candidate = i + j;
                            if (candidate >= N) continue;

                            double original = Y[candidate];
                            double calculated = 0;

                            // Вычисляем корректное значение
                            if (j == 0) {
                                calculated = Y[i + 3] - 3 * Y[i + 2] + 3 * Y[i + 1] - const_d3;
                            }
                            else if (j == 1) {
                                calculated = (const_d3 - Y[i + 3] + 3 * Y[i + 2] + Y[i]) / 3.0;
                            }
                            else if (j == 2) {
                                calculated = (Y[i + 3] + 3 * Y[i + 1] - Y[i] - const_d3) / 3.0;
                            }
                            else if (j == 3) {
                                calculated = const_d3 + 3 * Y[i + 2] - 3 * Y[i + 1] + Y[i];
                            }


                            // Временное исправление для проверки
                            Y[candidate] = calculated;
                            auto new_d3 = calcThirdDifferences();
                            bool valid = true;

                            for (auto val : new_d3) {
                                if (fabs(val - const_d3) > EPS) {
                                    valid = false;
                                    break;
                                }
                            }


                            if (valid) {
                                // Сохраняем первое успешное исправление
                                if (!error_fixed) {
                                    final_const = const_d3;
                                    error_index = candidate;
                                    corrected_value = calculated;
                                    error_fixed = true;
                                }
                                // Если найдены конфликтующие исправления
                                else if (error_index != candidate ||
                                    fabs(corrected_value - calculated) > EPS) {
                                    cout << "\nКонфликт исправлений:\nYY[" << error_index << "]: " << corrected_value << "\nYY["
                                        << candidate << "]: " << calculated << endl;
                                    IER = 2;
                                    Y[candidate] = original;
                                    return;
                                }
                            }
                            Y[candidate] = original;
                        }
                    }
                }
            }
        }
        

        if (error_fixed) {
            YY = Y;
            YY[error_index] = corrected_value;
            IER = 0;
        }
        else {
            // Проверяем случай когда ошибок нет
            bool all_same = true;
            const double first = d3[0];
            for (size_t i = 1; i < d3.size(); ++i) {
                if (fabs(d3[i] - first) > EPS) {
                    all_same = false;
                    break;
                }
            }
            IER = all_same ? 1 : 2;
        }

        if (IER == 0 || IER == 1) {
            calculateCoeff();
        }
    }

    // Вывод таблицы
    void print() const {
        cout << "\nX: ";
        for (auto x : X) printf("%8.4f ", x);
        cout << "\nY: ";
        for (auto y : Y) printf("%8.4f ", y);
        cout << "\n";
    }


    void printFinal() const {
        cout << "\nIER: " << IER;
        switch (IER)
        {
        case (0):
            cout << " (ошибка обнаружена)\n";
            break;
        case (1):
            cout << " (ошибки нет)\n";
            break;
        case (2):
            cout << " (мало точек)\n";
            break;
        case (3):
            cout << " (нарушен порядок возрастания аргумента в векторе X)\n";
            break;
        }

        cout << "\nX:  ";
        for (auto x : X) printf("%8.4f ", x);
        cout << "\nYY: ";
        for (auto y : YY) printf("%8.4f ", y);
        
        cout << endl;
        if (IER == 0 || IER == 1)
        {
            cout << "\nКоэффициенты многочлена: \n";
            cout << "C0 = " << coeff[0] << "\n"
                << "C1 = " << coeff[1] << "\n"
                << "C2 = " << coeff[2] << "\n"
                << "C3 = " << coeff[3] << "\n";
            
        }
        
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    string filename;
    cout << "Введите название файла: ";
    cin >> filename;

    Table table;
    if (!table.loadFromFile(filename + ".txt")) {
        cerr << "Ошибка при открывании файла!\n";
        return 1;
    }

    table.print();
    table.findAndFixError();
    table.printFinal();

    return 0;
}