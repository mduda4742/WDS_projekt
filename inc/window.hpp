#ifndef WINDOW
#define WINDOW

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class Window : public QMainWindow {
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    virtual ~Window() = default;

private:
    QPushButton *m_button;
    QLabel *m_label;
    QVBoxLayout *m_layout;
    QWidget *m_centralWidget;
};

#endif