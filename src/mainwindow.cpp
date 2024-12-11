#include "mainwindow.h"
#include <QHBoxLayout>
#include "settings.h"
#include <iostream>

MainWindow::MainWindow() : QWidget(nullptr), glRenderer(nullptr)
{
    // Create main renderer
    glRenderer = new GLRenderer(this);
    glRenderer->setMinimumSize(1920, 1080);

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
    QLabel *fov_label = new QLabel();
    fov_label->setText("Field of View:");
    QLabel *time_label = new QLabel();
    time_label->setText("Time:");

    // Create parameter layouts
    QGroupBox *fovLayout = new QGroupBox();
    QHBoxLayout *lfov = new QHBoxLayout();
    QGroupBox *timeLayout = new QGroupBox();
    QHBoxLayout *ltime = new QHBoxLayout();

    // Create sliders and spinboxes
    createSliderSpinbox(fovSlider, fovBox, 10, 179, 45);
    createSliderSpinbox(timeSlider, timeBox, 0, 240, 120);

    // Create Time controls
    lfov->addWidget(fovSlider);
    lfov->addWidget(fovBox);
    fovLayout->setLayout(lfov);

    ltime->addWidget(timeSlider);
    ltime->addWidget(timeBox);
    timeLayout->setLayout(ltime);
    
    vLayout->addWidget(fov_label);
    vLayout->addWidget(fovLayout);
    vLayout->addWidget(time_label);
    vLayout->addWidget(timeLayout);

    // Create and add weather controls
    createWeatherControls();

    // Connect all UI elements
    connectUIElements();
    setupWeatherControls();

    // Create and add mountain controls
    createMountainControls();

    // Connect all UI elements
    connectUIElements();
    setupWeatherControls();
    setupMountainControls();

    // Initialize settings
    initSettings();
}

void MainWindow::initSettings()
{
    settings.fov = fovSlider->value();
    settings.time = timeSlider->value();

    if (noWeatherButton->isChecked()) {
        settings.weather = WeatherType::CLEAR;
    } else if (snowButton->isChecked()) {
        settings.weather = WeatherType::SNOW;
    } else if (rainButton->isChecked()) {
        settings.weather = WeatherType::RAIN;
    }

    if (snowMountainButton->isChecked()) {
        settings.mountain = MountainType::SNOW_MOUNTAIN;
    } else if (rockMountainButton->isChecked()) {
        settings.mountain = MountainType::ROCK_MOUNTAIN;
    } else {
        settings.mountain = MountainType::GRASS_MOUNTAIN;
    }
}

void MainWindow::createWeatherControls() {
    QLabel *weather_label = new QLabel("Weather Type:", this);
    QFont font = weather_label->font();
    font.setPointSize(12);
    font.setBold(true);
    weather_label->setFont(font);

    QGroupBox *weatherBox = new QGroupBox(this);
    QVBoxLayout *weatherLayout = new QVBoxLayout(weatherBox);

    noWeatherButton = new QRadioButton("No Weather", this);
    snowButton = new QRadioButton("Snow", this);
    rainButton = new QRadioButton("Rain", this);

    snowButton->setChecked(true);

    weatherLayout->addWidget(noWeatherButton);
    weatherLayout->addWidget(snowButton);
    weatherLayout->addWidget(rainButton);

    vLayout->addWidget(weather_label);
    vLayout->addWidget(weatherBox);
}

void MainWindow::setupWeatherControls() {
    if (!noWeatherButton || !snowButton || !rainButton || !glRenderer) return;

    connect(noWeatherButton, &QRadioButton::toggled,
            this, &MainWindow::onWeatherTypeChanged,
            Qt::ConnectionType::QueuedConnection);
    connect(rainButton, &QRadioButton::toggled,
            this, &MainWindow::onWeatherTypeChanged,
            Qt::ConnectionType::QueuedConnection);
    connect(snowButton, &QRadioButton::toggled,
            this, &MainWindow::onWeatherTypeChanged,
            Qt::ConnectionType::QueuedConnection);
}

void MainWindow::onWeatherTypeChanged() {
    if (!glRenderer) return;

    std::cout << "No Weather: "<< noWeatherButton->isChecked() << std::endl;
    std::cout << "Snow: "<< snowButton->isChecked() << std::endl;
    std::cout << "Rain: "<< rainButton->isChecked() << std::endl;
    if (noWeatherButton->isChecked()) {
        settings.weather = WeatherType::CLEAR;
    } else if (snowButton->isChecked()) {
        settings.weather = WeatherType::SNOW;
    } else {
        settings.weather = WeatherType::RAIN;
    }

    glRenderer->settingsChanged();
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
    // spinbox->setDecimals(1);  // Allows decimal input
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
    settings.time = timeSlider->value() / 10.0f;
    if (glRenderer) glRenderer->settingsChanged();
}

void MainWindow::onValChangeFov(int newValue) {
    if (!fovSlider || !fovBox) return;

    fovSlider->setValue(newValue);
    fovBox->setValue(newValue);
    settings.fov = fovSlider->value();
    if (glRenderer) glRenderer->settingsChanged();
}

//mountain
void MainWindow::createMountainControls() {
    QLabel *mountain_label = new QLabel("Mountain Type:", this);
    QFont font = mountain_label->font();
    font.setPointSize(12);
    font.setBold(true);
    mountain_label->setFont(font);

    QGroupBox *mountainBox = new QGroupBox(this);
    QVBoxLayout *mountainLayout = new QVBoxLayout(mountainBox);

    snowMountainButton = new QRadioButton("Snow Mountain", this);
    rockMountainButton = new QRadioButton("Rock Mountain", this);
    grassMountainButton = new QRadioButton("Grass Mountain", this);

    snowMountainButton->setChecked(true);

    mountainLayout->addWidget(snowMountainButton);
    mountainLayout->addWidget(rockMountainButton);
    mountainLayout->addWidget(grassMountainButton);

    vLayout->addWidget(mountain_label);
    vLayout->addWidget(mountainBox);
}

void MainWindow::setupMountainControls() {
    if (!snowMountainButton || !rockMountainButton || !grassMountainButton || !glRenderer) return;

    connect(snowMountainButton, &QRadioButton::toggled,
            this, &MainWindow::onMountainTypeChanged,
            Qt::ConnectionType::QueuedConnection);
    connect(rockMountainButton, &QRadioButton::toggled,
            this, &MainWindow::onMountainTypeChanged,
            Qt::ConnectionType::QueuedConnection);
    connect(grassMountainButton, &QRadioButton::toggled,
            this, &MainWindow::onMountainTypeChanged,
            Qt::ConnectionType::QueuedConnection);
}
void MainWindow::onMountainTypeChanged() {
    if (!glRenderer) return;

    if (snowMountainButton->isChecked()) {
        settings.mountain = MountainType::SNOW_MOUNTAIN;
    } else if (rockMountainButton->isChecked()) {
        settings.mountain = MountainType::ROCK_MOUNTAIN;
    } else if (grassMountainButton->isChecked()) {
        settings.mountain = MountainType::GRASS_MOUNTAIN;
    }

    glRenderer->settingsChanged();
}



MainWindow::~MainWindow() {
    delete glRenderer;
}


