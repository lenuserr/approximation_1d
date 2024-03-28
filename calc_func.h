#include <QVector>

void CalculateAlpha(int n, double a, double b,
    const QVector<double>& x, const QVector<double>& y, QVector<double>* alpha);
void FillAlpha(int n, double a, double b, double x, double y, QVector<double>* alpha);
double Pf(int n, double x, double a, double b, const QVector<double>& alpha);
