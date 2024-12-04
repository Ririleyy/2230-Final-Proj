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
    QLabel *param1_label = new QLabel();
    param1_label->setText("Elevation:");
    QLabel *param2_label = new QLabel();
    param2_label->setText("Azimuth:");

    // Create parameter layouts
    QGroupBox *p1Layout = new QGroupBox();
    QHBoxLayout *l1 = new QHBoxLayout();
    QGroupBox *p2Layout = new QGroupBox();
    QHBoxLayout *l2 = new QHBoxLayout();

    // Create sliders and spinboxes
    p1Slider = new QSlider(Qt::Orientation::Horizontal);
    p1Slider->setTickInterval(1);
    p1Slider->setMinimum(0);
    p1Slider->setMaximum(180);
    p1Slider->setValue(0);

    p1Box = new QSpinBox();
    p1Box->setMinimum(0);
    p1Box->setMaximum(180);
    p1Box->setSingleStep(1);
    p1Box->setValue(0);

    p2Slider = new QSlider(Qt::Orientation::Horizontal);
    p2Slider->setTickInterval(1);
    p2Slider->setMinimum(0);
    p2Slider->setMaximum(360);
    p2Slider->setValue(0);

    p2Box = new QSpinBox();
    p2Box->setMinimum(0);
    p2Box->setMaximum(360);
    p2Box->setSingleStep(1);
    p2Box->setValue(0);

    // Add widgets to layouts
    l1->addWidget(p1Slider);
    l1->addWidget(p1Box);
    p1Layout->setLayout(l1);

    l2->addWidget(p2Slider);
    l2->addWidget(p2Box);
    p2Layout->setLayout(l2);

    // Add everything to vertical layout
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(param2_label);
    vLayout->addWidget(p2Layout);

    connectUIElements();
}

void MainWindow::connectUIElements() {
    connectParam1();
    connectParam2();
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::connectParam2() {
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}

void MainWindow::onValChangeP1(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.azimuth = p1Slider->value();
    glRenderer->settingsChanged();
}

void MainWindow::onValChangeP2(int newValue) {
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.elevation = p2Slider->value();
    glRenderer->settingsChanged();
}

MainWindow::~MainWindow() {
    delete glRenderer;
}

