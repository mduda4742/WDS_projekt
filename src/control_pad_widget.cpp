#include "control_pad_widget.hpp"
#include <QVariant>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>

ControlPadWidget::ControlPadWidget(QWidget *parent) : QWidget(parent) {

    setFixedSize(200, 200);
    setStyleSheet("border: 1px solid black; background-color: transparent;");

    auto *layout = new QGridLayout(this);
    layout->setSpacing(3);               // Odstępy między przyciskami
    layout->setContentsMargins(5, 5, 5, 5); // Marginesy wewnętrzne panelu

    publishTimer = new QTimer(this);
    connect(publishTimer, &QTimer::timeout, this, &ControlPadWidget::sendVelocityCommand);

    
    const QString buttonStyle = R"(
        QPushButton {
            background-color: #4CAF50;  /* Zielony */
            color: white;
            border: 2px solid #2E7D32;  /* Ciemniejsza ramka */
            border-radius: 5px;
            font-weight: bold;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #ff9800;  /* Pomarańczowy przy kliknięciu */
            border: 2px solid #f57c00;
        }
    )";

    // 4. Definicja przycisków: {Symbol, Wiersz, Kolumna, Kierunek_X, Kierunek_Y}
    struct BtnDef { QString icon; int row; int col; double v_lin; double v_ang; };
    QList<BtnDef> btnDefs = {
        {"↖", 0, 0, 1, 1},  {"↑", 0, 1, 1, 0},  {"↗", 0, 2, 1, -1},
        {"←", 1, 0, 0, 1},  {"●", 1, 1, 0, 0},  {"→", 1, 2, 0, -1},
        {"↙", 2, 0, -1, -1}, {"↓", 2, 1, -1, 0}, {"↘", 2, 2, -1, 1}
    };

    for (const auto &d : btnDefs) {
        auto *btn = new QPushButton(d.icon, this);
        btn->setFixedSize(40, 40);
        btn->setStyleSheet(buttonStyle);

        // Przechowujemy wartości prędkości wewnątrz przycisku
        btn->setProperty("vx", d.v_lin * linear_speed_);
        btn->setProperty("vth", d.v_ang * angular_speed_);

        layout->addWidget(btn, d.row, d.col);

        // Łączymy sygnały
        connect(btn, &QPushButton::pressed, this, &ControlPadWidget::onButtonPressed);
        connect(btn, &QPushButton::released, this, &ControlPadWidget::onButtonReleased);
    }
}

void ControlPadWidget::onButtonPressed() {
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        current_vx = btn->property("vx").toDouble();
        current_vth = btn->property("vth").toDouble();

        sendVelocityCommand();
        if (!publishTimer->isActive()) publishTimer->start(100);
    }
}

void ControlPadWidget::onButtonReleased() {
    current_vx = 0.0;
    current_vth = 0.0;

    sendVelocityCommand();
    publishTimer->stop();
}

void ControlPadWidget::sendVelocityCommand() {
    emit velocityRequested(current_vx, current_vth);
}