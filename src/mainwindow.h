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
    QRadioButton *snowButton;
    QRadioButton *rainButton;
    QVBoxLayout *vLayout;

    // Helper methods
    void createWeatherControls();
    void connectUIElements();
    void connectTime();
    void connectFov();
    void setupWeatherControls();
    void createSliderSpinbox(QSlider *&slider, QSpinBox *&spinbox, int min, int max, int defaultVal);

    // Event handlers
    void onValChangeFov(int newValue);
    void onValChangeTime(int newValue);
    void onWeatherTypeChanged();
};
