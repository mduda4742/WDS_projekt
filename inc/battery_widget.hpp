#ifndef BATTERY_WIDGET_H
#define BATTERY_WIDGET_H

#include <QWidget>

/**
 * @class BatteryWidget
 * @brief A custom UI widget that visually displays the robot's battery level.
 * 
 * This class handles custom rendering of a battery icon, updating its visual fill
 * state dynamically as the charge level changes.
 */

class BatteryWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for BatteryWidget.
     * @param parent - pointer to the parent widget (default is nullptr).
     */
    explicit BatteryWidget(QWidget *parent = nullptr);

    /** 
     * @brief Sets the current battery level and triggers a UI refresh.
     * @param level - value ranging from 0.0 (completely empty) to 1.0 (fully charged).
     */
    void setLevel(double level);

protected:
    /**
     * @brief Overridden paint event handler to draw the custom battery icon components.
     * @param event - pointer to the incoming paint event.
     */
    void paintEvent(QPaintEvent *event) override;

private:
    double m_level;     /**< Current battery level (0.0 to 1.0) used for rendering the fill state of the icon. */
};

#endif 
