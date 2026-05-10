#ifndef BATTERY_WIDGET_H
#define BATTERY_WIDGET_H

#include <QWidget>

class BatteryWidget : public QWidget {
    Q_OBJECT

public:
    explicit BatteryWidget(QWidget *parent = nullptr);

    /** 
     * @brief Set battery level
     * @param level Value from 0.0 (empty) to 1.0 (full)
     */
    void setLevel(double level);

protected:
    /**
     * @brief Method to print a widget
     */
    void paintEvent(QPaintEvent *event) override;

private:
    double m_level;     // battery level
};

#endif // BATTERY_WIDGET_H
