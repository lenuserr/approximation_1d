#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QtWidgets>

class Window : public QWidget {
    Q_OBJECT

private:
    int func_id;
    const char* f_name;
    double a;
    double b;
    int n;
    int k;
    int p = 0;
    double (*f) (double);

public:
    Window (QWidget *parent);

    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    int parse_command_line (int argc, char *argv[]);
    void select_f();
     QPointF l2g (double x_loc, double y_loc, double y_min, double y_max);
public slots:
    void previous_func();
    void next_func ();
    void increase_n();
    void decrease_n();
    void increase_f();
    void decrease_f();

protected:
    void paintEvent (QPaintEvent *event);
};

#endif
