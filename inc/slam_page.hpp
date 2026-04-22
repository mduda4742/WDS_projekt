#ifndef SLAM_PAGE_HPP
#define SLAM_PAGE_HPP

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

class SlamPage : public QWidget {
    Q_OBJECT

public:
    explicit SlamPage(QWidget *parent = nullptr);
    virtual ~SlamPage() = default;

public slots:
    void updateMapData(const QString &msg);

private:
    QLabel *mapPlaceholder;

    void setupControls(QGridLayout *layout);
};

#endif
