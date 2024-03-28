#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QtWidgets>
#include <QVector>

class Window : public QWidget {
    Q_OBJECT

private:
    int func_id;
    const char *f_name;
    double a;
    double b;
    int n;
    int k;
    double (*f) (double);

public:
    Window (QWidget *parent);

    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    int parse_command_line (int argc, char *argv[]);
    double Talpha(int n, double x, double y, QVector<double>* alpha);
    double Pf(int n, double x, const QVector<double>& alpha);
     QPointF l2g (double x_loc, double y_loc, double y_min, double y_max);
public slots:
    void change_func ();

protected:
    void paintEvent (QPaintEvent *event);
};

#endif
