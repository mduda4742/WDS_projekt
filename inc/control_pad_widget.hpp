#ifndef CONTROL_PAD_WIDGET_HPP
#define CONTROL_PAD_WIDGET_HPP

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>

class ControlPadWidget : public QWidget {
    Q_OBJECT
public:
    explicit ControlPadWidget(QWidget *parent = nullptr);

signals:
    // Definiujemy sygnał, który wysyła prędkości (x, y, yaw)
    void velocityRequested(double vx, double vy, double vth);

private slots:
    void onButtonPressed();
    void onButtonReleased();
    void sendVelocityCommand();

private:
    QTimer *publishTimer;
    double current_vx = 0.0;
    double current_vth = 0.0;

    const double linear_speed_ = 0.5;  // Stała prędkość liniowa
    const double angular_speed_ = 1.0; // Stała prędkość
};

#endif // CONTROL_PAD_WIDGET_HPP