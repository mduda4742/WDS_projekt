#ifndef SLAM_PAGE_HPP
#define SLAM_PAGE_HPP

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QPainter>
#include <vector>

class MapWidget : public QWidget {
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);
    virtual ~MapWidget() = default;

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void updateLaserScan(const std::vector<float> &ranges, 
                         float angle_min, float angle_max, float angle_increment);

private:
    std::vector<float> ranges_;
    float angle_min_;
    float angle_max_;
    float angle_increment_;
    bool hasData_;
    
    void drawLaserScan(QPainter &painter);
};

class SlamPage : public QWidget {
    Q_OBJECT

public:
    explicit SlamPage(QWidget *parent = nullptr);
    virtual ~SlamPage() = default;

public slots:
    void updateMapData(const QString &msg);
    void updateLaserData(const std::vector<float> &ranges, 
                        float angle_min, float angle_max, float angle_increment);

private:
    MapWidget *mapWidget;

    void setupControls(QGridLayout *layout);
};

#endif
