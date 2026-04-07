#ifndef WINDOW
#define WINDOW

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QGridLayout>
#include <QTabWidget>

class Window : public QMainWindow {
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    virtual ~Window() = default;

public slots:
    void updateTestData(const QString &msg);

private:
    QTabWidget *tabWidget;

    QWidget *createHomePage();
    QWidget *createSlamPage();
    QWidget *createImuPage();

    QLabel *mapPlaceholder;
    QLabel *accelLabel;
    QLabel *gyroLabel;
};

#endif