#include "calc_func.h"

void CalculateAlpha(int n, double a, double b,
    const QVector<double>& x, const QVector<double>& y, QVector<double>* alpha) {

    for (int j = 0; j < n; ++j) {
        FillAlpha(n, a, b, x[j], y[j], alpha);
    }

    (*alpha)[0] /= n;
    for (int i = 1; i < n; ++i) {
        (*alpha)[i] *= 2;
        (*alpha)[i] /= n;
    }    
}

void FillAlpha(int n, double a, double b, double x, double y, QVector<double>* alpha) {
    double T0 = 1;
    double T1 = (2*x - (b + a)) / (b - a);
    (*alpha)[0] += y;
    if (n == 1) {
        return;
    }

    (*alpha)[1] += T1 * y;
    double var = T1;
    double res = 0;
    for (int k = 2; k < n; ++k) {
        res = 2*var*T1 - T0;
        T0 = T1;
        T1 = res;
        (*alpha)[k] += res * y;
    }
}

double Pf(int n, double x, double a, double b, const QVector<double>& alpha) {
    double T0 = 1;
    double T1 = (2*x - (b + a)) / (b - a);
    double Pf_x = alpha[0] * T0;
    if (n == 1) {
        return Pf_x;
    } else if (n == 2) {
        return Pf_x + alpha[1] * T1;
    }

    Pf_x += alpha[1] * T1;
    double var = T1;
    double res = 0;
    for (int k = 2; k < n; ++k) {
        res = 2*var*T1 - T0;
        T0 = T1;
        T1 = res;
        Pf_x += alpha[k] * res;
    }

    return Pf_x;
}
