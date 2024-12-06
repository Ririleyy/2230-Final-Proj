#include "mainwindow.h"
#include <QHBoxLayout>
#include "settings.h"

MainWindow::MainWindow() : QWidget(nullptr), glRenderer(nullptr)
{
    // Create main renderer
    glRenderer = new GLRenderer(this);

    // Create main layouts
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    vLayout = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);

    // Add layouts
    hLayout->addLayout(vLayout);
    hLayout->addWidget(glRenderer, 1);

    // Create font for labels
    QFont font;
    font.setPointSize(12);
    font.setBold(true);

    // Create FOV controls
    QLabel *fov_label = new QLabel("Field of View:", this);
    fov_label->setFont(font);
    QGroupBox *fovLayout = new QGroupBox(this);
    QHBoxLayout *lfov = new QHBoxLayout(fovLayout);
    createSliderSpinbox(fovSlider, fovBox, 10, 179, 45);
    lfov->addWidget(fovSlider);
    lfov->addWidget(fovBox);

    // Create Time controls
    QLabel *time_label = new QLabel("Time:", this);
    time_label->setFont(font);
    QGroupBox *timeLayout = new QGroupBox(this);
    QHBoxLayout *ltime = new QHBoxLayout(timeLayout);
    createSliderSpinbox(timeSlider, timeBox, 0, 24, 12);
    ltime->addWidget(timeSlider);
    ltime->addWidget(timeBox);

    // Add FOV and Time controls to main layout
    vLayout->addWidget(fov_label);
    vLayout->addWidget(fovLayout);
    vLayout->addWidget(time_label);
    vLayout->addWidget(timeLayout);

    // Create and add weather controls
    createWeatherControls();

    // Connect all UI elements
    connectUIElements();
    setupWeatherControls();
}

void MainWindow::createWeatherControls() {
    QLabel *weather_label = new QLabel("Weather Type:", this);
    QFont font = weather_label->font();
    font.setPointSize(12);
    font.setBold(true);
    weather_label->setFont(font);

    QGroupBox *weatherBox = new QGroupBox(this);
    QVBoxLayout *weatherLayout = new QVBoxLayout(weatherBox);

    snowButton = new QRadioButton("Snow", this);
    rainButton = new QRadioButton("Rain", this);
    snowButton->setChecked(true);

    weatherLayout->addWidget(snowButton);
    weatherLayout->addWidget(rainButton);

    vLayout->addWidget(weather_label);
    vLayout->addWidget(weatherBox);
}

void MainWindow::setupWeatherControls() {
    if (!snowButton || !rainButton || !glRenderer) return;

    connect(snowButton, &QRadioButton::toggled,
            this, &MainWindow::onWeatherTypeChanged,
            Qt::ConnectionType::QueuedConnection);
}

void MainWindow::onWeatherTypeChanged() {
    if (!snowButton || !glRenderer) return;

    bool isSnow = snowButton->isChecked();
    glRenderer->setWeatherType(isSnow);
}

void MainWindow::createSliderSpinbox(QSlider *&slider, QSpinBox *&spinbox, int min, int max, int defaultVal) {
    slider = new QSlider(Qt::Orientation::Horizontal, this);
    slider->setTickInterval(1);
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(defaultVal);

    spinbox = new QSpinBox(this);
    spinbox->setMinimum(min);
    spinbox->setMaximum(max);
    spinbox->setSingleStep(1);
    spinbox->setValue(defaultVal);
}

void MainWindow::connectUIElements() {
    if (!fovSlider || !fovBox || !timeSlider || !timeBox) return;

    connectFov();
    connectTime();
}

void MainWindow::connectTime() {
    connect(timeSlider, &QSlider::valueChanged,
            this, &MainWindow::onValChangeTime);
    connect(timeBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTime);
}

void MainWindow::connectFov() {
    connect(fovSlider, &QSlider::valueChanged,
            this, &MainWindow::onValChangeFov);
    connect(fovBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeFov);
}

void MainWindow::onValChangeTime(int newValue) {
    if (!timeSlider || !timeBox) return;

    timeSlider->setValue(newValue);
    timeBox->setValue(newValue);
    settings.time = timeSlider->value();
    if (glRenderer) glRenderer->settingsChanged();
}

void MainWindow::onValChangeFov(int newValue) {
    if (!fovSlider || !fovBox) return;

    fovSlider->setValue(newValue);
    fovBox->setValue(newValue);
    settings.fov = fovSlider->value();
    if (glRenderer) glRenderer->settingsChanged();
}

MainWindow::~MainWindow() {
    delete glRenderer;
}
