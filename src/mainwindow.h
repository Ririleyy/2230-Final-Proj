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
    // QSlider *p1Slider;
    // QSlider *p2Slider;
    // QSpinBox *p1Box;
    // QSpinBox *p2Box;
    QSlider *fovSlider;
    QSpinBox *fovBox;
    QSlider *timeSlider;
    QSpinBox *timeBox;
    QSlider *turbiditySlider;
    QSpinBox *turbidityBox;

    void connectUIElements();
    // void connectParam1();
    // void connectParam2();
    void connectTime();
    void connectFov();
    void connectTurbidity();
    // void onValChangeP1(int newValue);
    // void onValChangeP2(int newValue);
    void onValChangeFov(int newValue);
    void onValChangeTime(int newValue);
    void onValChangeTurbidity(int newValue);
    void createSliderSpinbox(QSlider *&slider, QSpinBox *&spinbox, int min, int max, int defaultVal);
};
