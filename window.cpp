
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

    x.reserve(n);
    for (int m = 1; m <= n; ++m) {
        x.push_back(0.5*(a + b) + 0.5*(b - a)*cos(M_PI*0.5*(2*m - 1) / n));
    }

    func_id = k - 1;
    change_func();
    return 0;
}

/// change current function for drawing
void Window::change_func () {
    func_id = (func_id + 1) % 7;

    switch (func_id) {
        case 0:
            f_name = "f (x) = 1";
            f = f_0;
            break;
        case 1:
            f_name = "f (x) = x";
            f = f_1;
            break;
        case 2:
            f_name = "f (x) = x * x";
            f = f_2;
            break;
        case 3:
            f_name = "f (x) = x * x * x";
            f = f_3;
            break;
        case 4:
            f_name = "f (x) = x * x * x * x";
            f = f_4;
            break;
        case 5:
            f_name = "f (x) = exp(x)";
            f = f_5;
            break;
        case 6:
            f_name = "f (x) = 1 / (25*x*x + 1)";
            f = f_6;
            break;
    }
    update ();
}

QPointF Window::l2g (double x_loc, double y_loc, double y_min, double y_max) {
    double x_gl = (x_loc - a) / (b - a) * width ();
    double y_gl = (y_max - y_loc) / (y_max - y_min) * height ();
    return QPointF (x_gl, y_gl);
}

/// render graph
void Window::paintEvent (QPaintEvent * /* event */) {
    QPainter painter (this);
    double x1, x2, y1, y2;
    double max_y, min_y;
    double delta_y;
    QPen pen_black(Qt::black, 0, Qt::SolidLine);
    QPen pen_red(Qt::red, 0, Qt::SolidLine);

    painter.setPen (pen_black);

    QVector<double> y(n);
    for (int i = 0; i < n; ++i) {
        y[i] = f(x[i]);
    }

    QVector<double> alpha(n);
    CalculateAlpha(n, a, b, x, y, &alpha);

    double delta_x = (b - a) / 20; // по 20 точкам пока порисую
    // calculate min and max for Pf
    /*
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
    */

    // calculate min and max for current function
    max_y = min_y = 0;
    for (auto v : y) {
        if (v < min_y)
            min_y = v;
        if (v > max_y)
            max_y = v;
    }

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

    // draw axis
    painter.setPen (pen_red);
    painter.drawLine (L2G(a, 0), L2G(b, 0));
    painter.drawLine (L2G(0, min_y), L2G(0, max_y));

    // render function name
    painter.setPen ("blue");
    painter.drawText (0, 20, f_name);
}
