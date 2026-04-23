#ifndef IMU_PAGE_HPP
#define IMU_PAGE_HPP

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class ImuPage : public QWidget {
    Q_OBJECT

public:
    explicit ImuPage(QWidget *parent = nullptr);
    virtual ~ImuPage() = default;

public slots:

    /*
    void updateAccelData(const QString &msg);
    void updateGyroData(const QString &msg);
    */

    void updateYaw(double yaw);
    void updateVoltage(double voltage);

private:
    QLabel *accelLabel;
    QLabel *gyroLabel;

    QLabel *cameraLabel;
    QLabel *batteryLabel;
    
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *yawLabel;

    QLabel *linearVelLabel;
    QLabel *angularVelLabel;
};

#endif
