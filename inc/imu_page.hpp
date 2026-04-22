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
    void updateAccelData(const QString &msg);
    void updateGyroData(const QString &msg);

private:
    QLabel *accelLabel;
    QLabel *gyroLabel;
};

#endif
