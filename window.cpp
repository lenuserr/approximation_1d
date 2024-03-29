// 21:12. 28.03.24. 3 hours coding let's go.
#include <QPainter>
#include <stdio.h>

#include "window.h"
#include "calc_func.h"

#define L2G(X,Y) (l2g ((X), (Y), min_y, max_y))

static double f_0 ([[maybe_unused]] double x) {
    return 1;
}

static double f_1 (double x) {
    return x;
}

static double f_2 (double x) {
    return x*x;
}

static double f_3 (double x) {
    return x*x*x;
}

static double f_4 (double x) {
    return x*x*x*x;
}

static double f_5 (double x) {
    return exp(x);
}

static double f_6 (double x) {
    return 1 / (25*x*x + 1);
}

static double df_0([[maybe_unused]] double x) {
    return 0;
}

static double df_1([[maybe_unused]] double x) {
    return 1;
}

static double df_2(double x) {
    return 2*x;
}

static double df_3(double x) {
    return 3*x*x;
}

static double df_4(double x) {
    return 4*x*x*x;
}

static double df_5(double x) {
    return exp(x);
}

static double df_6(double x) {
    return (-50*x) / ((25*x*x + 1) * (25*x*x + 1));
}

Window::Window (QWidget *parent)
    : QWidget (parent)
{}

QSize Window::minimumSizeHint () const {
    return QSize (100, 100);
}

QSize Window::sizeHint () const {
    return QSize (1000, 1000);
}

int Window::parse_command_line (int argc, char *argv[]) {
    if (argc != 5)
        return -1;

    if (   sscanf (argv[1], "%lf", &a) != 1
        || sscanf (argv[2], "%lf", &b) != 1
        || b - a < 1.e-6
        || sscanf (argv[3], "%d", &n) != 1
        || n <= 0
        || sscanf (argv[4], "%d", &k) != 1
        || k < 0 || k > 6)
        return -2;

    func_id = k - 1;
    next_func();
    return 0;
}

void Window::select_f() {
    switch (func_id) {
        case 0:
            f_name = "k = 0 f (x) = 1";
            f = f_0;
            df = df_0;
            break;
        case 1:
            f_name = "k = 1 f (x) = x";
            f = f_1;
            df = df_1;
            break;
        case 2:
            f_name = "k = 2 f (x) = x * x";
            f = f_2;
            df = df_2;
            break;
        case 3:
            f_name = "k = 3 f (x) = x * x * x";
            f = f_3;
            df = df_3;
            break;
        case 4:
            f_name = "k = 4 f (x) = x * x * x * x";
            f = f_4;
            df = df_4;
            break;
        case 5:
            f_name = "k = 5 f (x) = exp(x)";
            f = f_5;
            df = df_5;
            break;
        case 6:
            f_name = "k = 6 f (x) = 1 / (25*x*x + 1)";
            f = f_6;
            df = df_6;
            break;
    }
}

QPointF Window::l2g (double x_loc, double y_loc, double y_min, double y_max) {
    double x_gl = (x_loc - a) / (b - a) * width ();
    double y_gl = (y_max - y_loc) / (y_max - y_min) * height ();
    return QPointF (x_gl, y_gl);
}

void Window::previous_func () {
    func_id = (func_id - 1) % 7;
    if (!func_id) { func_id = 6; } // потом константу отрисую нормально

    select_f();
    update();
}

void Window::next_func () {
    func_id = (func_id + 1) % 7;
    if (!func_id) { func_id = 1; }

    select_f();
    update();
}

void Window::change_graph() {
    n_graph = (n_graph + 1) % 2;
    update();
}

void Window::increase_s() {
    s++;
    a /= 2;
    b /= 2;
    update();
}

void Window::decrease_s() {
    s--;
    a *= 2;
    b *= 2;
    update();
}

void Window::increase_n() {
    n *= 2;
    update();
}

void Window::decrease_n() {
    n /= 2;
    update();
}

void Window::increase_f() {
    p++;
    update();
}

void Window::decrease_f() {
    p--;
    update();
}

void Window::draw_spline(const QVector<double>& x) {
    QPainter painter (this);
    double x1, x2, y1, y2;
    double max_y, min_y;
    int N = 1000;
    double delta_y, delta_x = (b - a) / N;
    QPen pen_black(Qt::black, 0, Qt::SolidLine);
    QPen pen_red(Qt::red, 0, Qt::SolidLine);

    painter.setPen (pen_black);

    double* main_d = new double[n];
    double* B = new double[n];
    double* left_d = new double[n-1];
    double* right_d = new double[n-1];
    double* d = new double[n];

    construct_matrix(n, f, df, x, left_d, main_d, right_d, B);
    solution(n, main_d, left_d, right_d, B, d);


    double pt = (a + b) / 2;
    Sf(double pt, int n, const QVector<double>& x, double (*f) (double), double* d)

    delete[] main_d;
    delete[] B;
    delete[] left_d;
    delete[] right_d;
    delete[] d;
}

void Window::draw_residual(const QVector<double>& alpha) {
  QPainter painter (this);
  double x1, x2, y1, y2;
  double max_y, min_y;
  int N = 1000;
  double delta_y, delta_x = (b - a) / N;
  QPen pen_black(Qt::black, 0, Qt::SolidLine);
  QPen pen_red(Qt::red, 0, Qt::SolidLine);

  painter.setPen (pen_black);

  // calculate min and max for current function
  x1 = a;
  y1 = std::abs(Pf(n, x1, a, b, alpha) - f(x1));
  max_y = min_y = y1;
  for (x2 = x1 + delta_x; x2 - b < 1.e-6; x2 += delta_x) {
    y2 = std::abs(Pf(n, x2, a, b, alpha) - f(x2));
    // local coords are converted to draw coords
    max_y = std::max(max_y, y2);
    min_y = std::min(min_y, y2);
    x1 = x2, y1 = y2;
  }
  x2 = b;
  y2 = std::abs(Pf(n, x2, a, b, alpha) - f(x2));
  max_y = std::max(max_y, y2);
  min_y = std::min(min_y, y2);


  delta_y = 0.01 * (max_y - min_y);
  min_y -= delta_y;
  max_y += delta_y;

  // draw approximated line for graph
  x1 = a;
  y1 = std::abs(Pf(n, x1, a, b, alpha) - f(x1));
  for (x2 = x1 + delta_x; x2 - b < 1.e-6; x2 += delta_x)
    {
      y2 = std::abs(Pf(n, x2, a, b, alpha) - f(x2));
      // local coords are converted to draw coords
      painter.drawLine (L2G(x1, y1), L2G(x2, y2));

      x1 = x2, y1 = y2;
    }
  x2 = b;
  y2 = std::abs(Pf(n, x2, a, b, alpha) - f(x2));
  painter.drawLine (L2G(x1, y1), L2G(x2, y2));


  // draw axis
  painter.setPen (pen_red);
  painter.drawLine (L2G(a, 0), L2G(b, 0));
  painter.drawLine (L2G(0, min_y), L2G(0, max_y));


  // render function name
  painter.setPen ("blue");
  double max_module_f = std::max(std::abs(min_y), std::abs(max_y));
  std::string text1 = "max|Pf - f| = " + std::to_string(max_module_f);
  painter.drawText (5, 20, f_name);
  painter.drawText(5, 40, text1.c_str());
}

void Window::print_warning() {
    QPainter painter (this);
    painter.setPen ("blue");
    std::string text1 = "Warning: This method does not work when n > 50";
    std::string text2 = "n = " + std::to_string(n);
    painter.drawText (5, 20, f_name);
    painter.drawText(5, 40, text1.c_str());
    painter.drawText(5, 60, text2.c_str());
}

/// render graph
void Window::paintEvent (QPaintEvent * /* event */) {
    if (n > 50) {
        print_warning();
        return;
    }

    double x1, x2, y1, y2;
    double max_y, min_y;
    double delta_y;

    QVector<double> x(n), y(n);
    max_y = std::abs(f(0.5*(a + b) + 0.5*(b - a)*cos(M_PI*0.5 / n)));
    for (int m = 1; m <= n; ++m) {
        x[m-1] = 0.5*(a + b) + 0.5*(b - a)*cos(M_PI*0.5*(2*m - 1) / n);
        y[m-1] = f(x[m-1]);
        max_y = std::max(max_y, std::abs(y[m-1]));
    }

    y[n/2] += p*0.1*max_y;

    QVector<double> alpha(n);
    CalculateAlpha(n, a, b, x, y, &alpha);

    if (n_graph == 1) {
        draw_residual(alpha);
        return;
    }

    QPainter painter (this);
    QPen pen_black(Qt::black, 0, Qt::SolidLine);
    QPen pen_red(Qt::red, 0, Qt::SolidLine);
    painter.setPen (pen_black);

    int N = 1000; // N точек для рисования.
    double delta_x = (b - a) / N;
    // calculate min and max for Pf
    x1 = a;
    y1 = Pf(n, x1, a, b, alpha);
    max_y = min_y = y1;
    for (x2 = x1 + delta_x; x2 - b < 1.e-6; x2 += delta_x) {
      y2 = Pf(n, x2, a, b, alpha);
      max_y = std::max(max_y, y2);
      min_y = std::min(min_y, y2);
      x1 = x2, y1 = y2;
    }
    x2 = b;
    y2 = Pf(n, x2, a, b, alpha);
    max_y = std::max(max_y, y2);
    min_y = std::min(min_y, y2);

    delta_y = 0.01 * (max_y - min_y);
    min_y -= delta_y;
    max_y += delta_y;

    // draw approximated line for graph
    x1 = a;
    y1 = Pf(n, x1, a, b, alpha);
    for (x2 = x1 + delta_x; x2 - b < 1.e-6; x2 += delta_x) {
      y2 = Pf(n, x2, a, b, alpha);
      // local coords are converted to draw coords
      painter.drawLine (L2G(x1, y1), L2G(x2, y2));

      x1 = x2, y1 = y2;
    }
    x2 = b;
    y2 = Pf(n, x2, a, b, alpha);
    painter.drawLine (L2G(x1, y1), L2G(x2, y2));

    //////////////// draw f //////////////////////////
    // calculate min and max for current function
    max_y = min_y = 0;
    for (x1 = a; x1 - b < 1.e-6; x1 += delta_x)
      {
        y1 = f (x1);
        if (y1 < min_y)
          min_y = y1;
        if (y1 > max_y)
          max_y = y1;
      }

    delta_y = 0.01 * (max_y - min_y);
    min_y -= delta_y;
    max_y += delta_y;

    // draw approximated line for graph
    x1 = a;
    y1 = f (x1);
    for (x2 = x1 + delta_x; x2 - b < 1.e-6; x2 += delta_x)
      {
        y2 = f (x2);
        // local coords are converted to draw coords
        painter.drawLine (L2G(x1, y1), L2G(x2, y2));

        x1 = x2, y1 = y2;
      }
    x2 = b;
    y2 = f (x2);
    painter.drawLine (L2G(x1, y1), L2G(x2, y2));
    //////////////// draw f //////////////////////////

    // draw axis
    painter.setPen (pen_red);
    painter.drawLine (L2G(a, 0), L2G(b, 0));
    painter.drawLine (L2G(0, min_y), L2G(0, max_y));

    // render function name
    painter.setPen ("blue");
    double max_module_f = std::max(std::abs(min_y), std::abs(max_y));
    std::string text1 = "max|f| = " + std::to_string(max_module_f);
    std::string text2 = "n = " + std::to_string(n);
    std::string text3 = "s = " + std::to_string(s);
    std::string text4 = "p = " + std::to_string(p);
    painter.drawText (5, 20, f_name);
    painter.drawText(5, 40, text1.c_str());
    painter.drawText(5, 60, text2.c_str());
    painter.drawText(5, 80, text3.c_str());
    painter.drawText(5, 100, text4.c_str());
}
