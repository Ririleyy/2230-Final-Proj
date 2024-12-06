#pragma once
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QMainWindow>
#pragma once

#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QMainWindow>
#include <QRadioButton>
#include <QVBoxLayout>
#include "glrenderer.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

private:
    // UI Components
    GLRenderer *glRenderer;
    QSlider *fovSlider;
    QSpinBox *fovBox;
    QSlider *timeSlider;
    QSpinBox *timeBox;
    QSlider *turbiditySlider;
    QSpinBox *turbidityBox;
    QRadioButton *snowButton;
    QRadioButton *rainButton;
    QVBoxLayout *vLayout;
    QRadioButton *noWeatherButton;

    // Helper methods
    void createWeatherControls();
    void connectUIElements();
    void connectTime();
    void connectFov();
    void connectTurbidity();
    // void onValChangeP1(int newValue);
    // void onValChangeP2(int newValue);
    void onValChangeFov(int newValue);
    void onValChangeTime(int newValue);
    void onValChangeTurbidity(int newValue);
    void onWeatherTypeChanged();
    void createSliderSpinbox(QSlider *&slider, QSpinBox *&spinbox, int min, int max, int defaultVal);
    void setupWeatherControls();
    // Event handlers
};
