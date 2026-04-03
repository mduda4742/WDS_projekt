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

private:
    QTabWidget *tabWidget;

    QWidget *createHomePage();
    QWidget *createSlamPage();
    QWidget *createImuPage();

    QLabel *accelLabel;
    QLabel *gyroLabel;
};

#endif