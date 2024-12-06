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
    QLabel *fov_label = new QLabel();
    fov_label->setText("Field of View:");
    QLabel *time_label = new QLabel();
    time_label->setText("Time:");
    QLabel *turbidity_label = new QLabel();
    turbidity_label->setText("Turbidity:");

    // Create parameter layouts
    QGroupBox *fovLayout = new QGroupBox();
    QHBoxLayout *lfov = new QHBoxLayout();
    QGroupBox *timeLayout = new QGroupBox();
    QHBoxLayout *ltime = new QHBoxLayout();
    QGroupBox *turbidityLayout = new QGroupBox();
    QHBoxLayout *lturbidity = new QHBoxLayout();

    // Create sliders and spinboxes
    createSliderSpinbox(fovSlider, fovBox, 10, 179, 45);
    createSliderSpinbox(timeSlider, timeBox, 0, 24, 12);
    createSliderSpinbox(turbiditySlider, turbidityBox, 1, 10, 1);

    // Create Time controls
    lfov->addWidget(fovSlider);
    lfov->addWidget(fovBox);
    fovLayout->setLayout(lfov);

    ltime->addWidget(timeSlider);
    ltime->addWidget(timeBox);
    timeLayout->setLayout(ltime);

    lturbidity->addWidget(turbiditySlider);
    lturbidity->addWidget(turbidityBox);
    turbidityLayout->setLayout(lturbidity);
    
    vLayout->addWidget(fov_label);
    vLayout->addWidget(fovLayout);
    vLayout->addWidget(time_label);
    vLayout->addWidget(timeLayout);
    vLayout->addWidget(turbidity_label);
    vLayout->addWidget(turbidityLayout);


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

    if (noWeatherButton->isChecked()) {
        glRenderer->setWeatherEnabled(false);
    } else {
        glRenderer->setWeatherEnabled(true);
        glRenderer->setWeatherType(snowButton->isChecked());
    }
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
    connectTurbidity();
}

void MainWindow::connectTime() {
    connect(timeSlider, &QSlider::valueChanged,
            this, &MainWindow::onValChangeTime);
    connect(timeBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTime);
}

void MainWindow::connectTurbidity() {
    connect(turbiditySlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTurbidity);
    connect(turbidityBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTurbidity);
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

void MainWindow::onValChangeTurbidity(int newValue) {
    turbiditySlider->setValue(newValue);
    turbidityBox->setValue(newValue);
    settings.T = turbiditySlider->value();
    glRenderer->settingsChanged();
}
MainWindow::~MainWindow() {
    delete glRenderer;
}
