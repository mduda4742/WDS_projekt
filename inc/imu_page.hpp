#ifndef IMU_PAGE_HPP
#define IMU_PAGE_HPP

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "odomState.hpp"

class ImuPage : public QWidget {
    Q_OBJECT

public:
    explicit ImuPage(QWidget *parent = nullptr);
    virtual ~ImuPage() = default;

public slots:

    void updateOdom(odomState state);
    void updateBattery(double voltage);

private:
    QLabel *cameraLabel;

    QLabel *batteryLabel;
    
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *yawLabel;

    QLabel *linearVelLabel;
    QLabel *angularVelLabel;
};

#endif
