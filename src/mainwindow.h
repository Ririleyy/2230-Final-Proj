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
    QRadioButton *noWeatherButton;

    QRadioButton *snowMountainButton;
    QRadioButton *rockMountainButton;
    QRadioButton *grassMountainButton;

    // Helper methods
    void createWeatherControls();
    void connectUIElements();
    void connectTime();
    void connectFov();
    void onValChangeFov(int newValue);
    void onValChangeTime(int newValue);
    void onWeatherTypeChanged();
    void createSliderSpinbox(QSlider *&slider, QSpinBox *&spinbox, int min, int max, int defaultVal);
    void setupWeatherControls();
    void initSettings();

    //mountain
    void createMountainControls();
    void setupMountainControls();
    void onMountainTypeChanged();
    // Event handlers
};
