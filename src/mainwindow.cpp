#include "mainwindow.h"

#include <QHBoxLayout>
#include "settings.h"

MainWindow::MainWindow()
{
    glRenderer = new GLRenderer;
    
    // Create main horizontal layout
    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment for controls
    vLayout->setAlignment(Qt::AlignTop);
    
    // Add layouts to main layout
    hLayout->addLayout(vLayout);
    hLayout->addWidget(glRenderer, 1);
    this->setLayout(hLayout);  // Only set the layout once

    // Create labels
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    // QLabel *param1_label = new QLabel();
    // param1_label->setText("Elevation:");
    // QLabel *param2_label = new QLabel();
    // param2_label->setText("Azimuth:");
    QLabel *fov_label = new QLabel();
    fov_label->setText("Field of View:");
    QLabel *time_label = new QLabel();
    time_label->setText("Time:");
    QLabel *turbidity_label = new QLabel();
    turbidity_label->setText("Turbidity:");

    // Create parameter layouts
    // QGroupBox *p1Layout = new QGroupBox();
    // QHBoxLayout *l1 = new QHBoxLayout();
    // QGroupBox *p2Layout = new QGroupBox();
    // QHBoxLayout *l2 = new QHBoxLayout();
    QGroupBox *fovLayout = new QGroupBox();
    QHBoxLayout *lfov = new QHBoxLayout();
    QGroupBox *timeLayout = new QGroupBox();
    QHBoxLayout *ltime = new QHBoxLayout();
    QGroupBox *turbidityLayout = new QGroupBox();
    QHBoxLayout *lturbidity = new QHBoxLayout();

    // Create sliders and spinboxes
    // createSliderSpinbox(p1Slider, p1Box, 0, 180, 0);
    // createSliderSpinbox(p2Slider, p2Box, 0, 360, 0);
    createSliderSpinbox(fovSlider, fovBox, 10, 179, 45);
    createSliderSpinbox(timeSlider, timeBox, 0, 24, 12);
    createSliderSpinbox(turbiditySlider, turbidityBox, 1, 10, 1);

    // Add widgets to layouts
    // l1->addWidget(p1Slider);
    // l1->addWidget(p1Box);
    // p1Layout->setLayout(l1);

    // l2->addWidget(p2Slider);
    // l2->addWidget(p2Box);
    // p2Layout->setLayout(l2);

    lfov->addWidget(fovSlider);
    lfov->addWidget(fovBox);
    fovLayout->setLayout(lfov);

    ltime->addWidget(timeSlider);
    ltime->addWidget(timeBox);
    timeLayout->setLayout(ltime);

    lturbidity->addWidget(turbiditySlider);
    lturbidity->addWidget(turbidityBox);
    turbidityLayout->setLayout(lturbidity);

    // Add everything to vertical layout
    // vLayout->addWidget(param1_label);
    // vLayout->addWidget(p1Layout);
    // vLayout->addWidget(param2_label);
    // vLayout->addWidget(p2Layout);
    vLayout->addWidget(fov_label);
    vLayout->addWidget(fovLayout);
    vLayout->addWidget(time_label);
    vLayout->addWidget(timeLayout);
    vLayout->addWidget(turbidity_label);
    vLayout->addWidget(turbidityLayout);




    connectUIElements();
}

void MainWindow::createSliderSpinbox(QSlider *&slider, QSpinBox *&spinbox, int min, int max, int defaultVal)
{
    slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setTickInterval(1);
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(defaultVal);

    spinbox = new QSpinBox();
    spinbox->setMinimum(min);
    spinbox->setMaximum(max);
    spinbox->setSingleStep(1);
    spinbox->setValue(defaultVal);
}

void MainWindow::connectUIElements() {
    // connectParam1();
    // connectParam2();
    connectFov();
    connectTime();
    connectTurbidity();
}

void MainWindow::connectTime() {
    connect(timeSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTime);
    connect(timeBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTime);
}

void MainWindow::connectTurbidity() {
    connect(turbiditySlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTurbidity);
    connect(turbidityBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTurbidity);
}

// void MainWindow::connectParam1() {
//     connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
//     connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
//             this, &MainWindow::onValChangeP1);
// }

// void MainWindow::connectParam2() {
//     connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
//     connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
//             this, &MainWindow::onValChangeP2);
// }

void MainWindow::connectFov() {
    connect(fovSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFov);
    connect(fovBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeFov);
}

void MainWindow::onValChangeTime(int newValue) {
    timeSlider->setValue(newValue);
    timeBox->setValue(newValue);
    settings.time = timeSlider->value();
    glRenderer->settingsChanged();
}

void MainWindow::onValChangeFov(int newValue) {
    fovSlider->setValue(newValue);
    fovBox->setValue(newValue);
    settings.fov = fovSlider->value();
    glRenderer->settingsChanged();
}

void MainWindow::onValChangeTurbidity(int newValue) {
    turbiditySlider->setValue(newValue);
    turbidityBox->setValue(newValue);
    settings.T = turbiditySlider->value();
    glRenderer->settingsChanged();
}

// void MainWindow::onValChangeP1(int newValue) {
//     p1Slider->setValue(newValue);
//     p1Box->setValue(newValue);
//     settings.azimuth = p1Slider->value();
//     glRenderer->settingsChanged();
// }

// void MainWindow::onValChangeP2(int newValue) {
//     p2Slider->setValue(newValue);
//     p2Box->setValue(newValue);
//     settings.elevation = p2Slider->value();
//     glRenderer->settingsChanged();
// }

MainWindow::~MainWindow() {
    delete glRenderer;
}

