#include "battery_widget.hpp"
#include <QPainter>
#include <QtGlobal>

BatteryWidget::BatteryWidget(QWidget *parent)
    : QWidget(parent), m_level(1.0) 
{
    setFixedSize(40, 20);
}

void BatteryWidget::setLevel(double level) {
    
    m_level = qBound(0.0, level, 1.0);

    update();
}

void BatteryWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Rysowanie obudowy
    QRectF body(2, 2, width() - 6, height() - 4);
    painter.setPen(QPen(Qt::lightGray, 2));
    painter.drawRoundedRect(body, 2, 2);

    // Rysowanie "cypla" (biegun dodatni)
    QRectF tip(width() - 4, height() / 3, 3, height() / 3);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(tip);

    // Wybór koloru (poniżej 20% robimy się czerwoni)
    QColor color = (m_level > 0.2) ? QColor("#00FF00") : QColor("#FF0000");
    
    // Rysowanie wypełnienia wnętrza
    double fillWidth = (body.width() - 4) * m_level;
    if (fillWidth > 0) {
        QRectF content(body.x() + 2, body.y() + 2, fillWidth, body.height() - 4);
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawRect(content);
    }
}
