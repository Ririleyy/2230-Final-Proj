#pragma once

#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QMainWindow>
#include "glrenderer.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    GLRenderer *glRenderer;
    QSlider *p1Slider;
    QSlider *p2Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;

    void connectUIElements();
    void connectParam1();
    void connectParam2();
    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);

};
